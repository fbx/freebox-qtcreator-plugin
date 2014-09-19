/*
  Freebox QtCreator plugin for QML application development.

  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not see
  http://www.gnu.org/licenses/lgpl-2.1.html.

  Copyright (c) 2014, Freebox SAS, See AUTHORS for details.
*/
#include <utils/qtcassert.h>

#include "http.hh"
#include "server.hh"
#include "client.hh"

namespace Freebox {
namespace Http {

QTextStream &operator<<(QTextStream &s, const IResponse &r)
{
    return s << r.code() << " " << r.message();
}

Server::Server(QObject *parent) :
    QTcpServer(parent)
{
}

void Server::incomingConnection(qintptr handle)
{
    Client *client = new Client(this);
    connect(client, SIGNAL(readyRead()), this, SLOT(readClient()));
    connect(client, SIGNAL(disconnected()), this, SLOT(disconnectClient()));
    client->setSocketDescriptor(handle);
}

void Server::reply(QTcpSocket *socket,
                   const Version &version,
                   const IResponse &response)
{
    QTextStream(socket) << version << " " << response << CRLF;
}

void Server::replyHeader(QTcpSocket *socket,
                         QString name,
                         QString value)
{
    QTextStream(socket) << name << ": " << value << CRLF;
}

void Server::replyHeaderEnd(QTcpSocket *socket)
{
    QTextStream(socket) << CRLF;
}

void Server::replyError(QTcpSocket *socket,
                        const Version &version,
                        const IResponse &response)
{
    reply(socket, version, response);
    replyHeaderEnd(socket);
    socket->close();
}

void Server::replyHeader(QTcpSocket *socket,
                         QString name,
                         unsigned int value)
{
    return replyHeader(socket, name, QString::number(value));
}

void Server::methodGet(Client *client,
                       Http::Method &,
                       QString &uri,
                       Http::Version &version,
                       QStringList &headers)
{
    Q_UNUSED(headers);

    qWarning() << "GET" << uri << version;

    if (uri[0] != QChar::fromAscii('/')) {
        qWarning("invalid URI");
        return replyError(client, version, Reply::BadRequest());
    }
    uri.remove(0, 1);

    if (mPath.absolutePath().isNull()) {
        qWarning("server path is not set.");
        return replyError(client, version, Reply::InternalServerError());
    }

    QFile file(mPath.absoluteFilePath(uri));
    if (!file.exists()) {
        qWarning("file %s does not exist", file.fileName().toUtf8().constData());
        return replyError(client, version, Reply::NotFound());
    }

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("fail to open file %s", file.fileName().toUtf8().constData());
        return replyError(client, version, Reply::InternalServerError());
    }

    reply(client, version, Reply::Ok());
    replyHeader(client, QStringLiteral("Content-Length"), file.size());
    replyHeader(client, QStringLiteral("Cache-Control"), QStringLiteral("no-cache"));
    replyHeader(client, QStringLiteral("Pragma"), QStringLiteral("no-cache"));
    replyHeaderEnd(client);

    while (!file.atEnd()) {
        QByteArray buffer = file.read(file.size());
        client->write(buffer);
    }

    client->close();
}

void Server::readClient()
{
    Client *client = qobject_cast<Client *>(sender());

    QTC_ASSERT(client, return);

    client->bufferedReadAll();
}

void Server::disconnectClient()
{
    Client *client = qobject_cast<Client *>(sender());

    QTC_ASSERT(client, return);

    client->deleteLater();
}

} // namespace Http
} // namespace Freebox
