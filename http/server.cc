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
#include <QFile>
#include <QUrl>
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
                         const QString &name,
                         const QString &value)
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
                         const QString &name,
                         unsigned int value)
{
    return replyHeader(socket, name, QString::number(value));
}

void Server::methodGet(Client *client,
                       const Http::Method &,
                       const QString &uri,
                       const Http::Version &version,
                       const QStringList &headers)
{
    Q_UNUSED(headers);

    qWarning() << "GET" << uri << version;

    if (mPath == QDir()) {
        qWarning() << "server path not set";
        return replyError(client, version, Reply::InternalServerError());
    }

    QUrl url(uri, QUrl::StrictMode);
    if (!url.isValid()) {
        qWarning() << "invalid URI" << qPrintable(uri);
        return replyError(client, version, Reply::BadRequest());
    }

    if (!url.isRelative()) {
        qWarning() << "unsupported URI" << qPrintable(uri);
        return replyError(client, version, Reply::BadRequest());
    }

    QString path = QDir::cleanPath(url.path());
    int slash = 0;
    while (path[slash] == QLatin1Char('/'))
        slash++;
    path.remove(0, slash);

    if (path.isEmpty()) {
        qWarning() << "cannot resolve path for" << qPrintable(uri);
        return replyError(client, version, Reply::BadRequest());
    }

    QFile file(mPath.absoluteFilePath(path));
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "failed to open file" << qUtf8Printable(file.fileName());
        return replyError(client, version, Reply::InternalServerError());
    }

    reply(client, version, Reply::Ok());
    replyHeader(client, QStringLiteral("Connection: close"), file.size());
    replyHeader(client, QStringLiteral("Content-Length"), file.size());
    replyHeader(client, QStringLiteral("Cache-Control"), QStringLiteral("no-cache"));
    replyHeader(client, QStringLiteral("Pragma"), QStringLiteral("no-cache"));
    replyHeader(client, QStringLiteral("Accept-Ranges"), QStringLiteral("none"));
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
