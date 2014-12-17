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
#include "http.hh"
#include "client.hh"
#include "server.hh"

namespace Freebox {
namespace Http {

Client::Client(QObject *parent) :
    QTcpSocket(parent),
    mLine(QStringLiteral("(\\w+)\\s+(.*)\\s+(HTTP/\\d\\.\\d)\\s*$"))
{
    connect(this, SIGNAL(request(QString)),
            this, SLOT(requestHandle(QString)));
}

void Client::bufferedReadAll()
{
    int pos;

    mBuffer += readAll();
    pos = mBuffer.indexOf(CRLFCRLF);

    if (pos) {
        request(QString::fromUtf8(mBuffer.left(pos)));
        mBuffer.remove(0, pos);
    }
}

bool Client::canReadHeader()
{
    return mBuffer.indexOf(CRLFCRLF);
}

void Client::requestHandle(QString request)
{
    QStringList headers = request.split(QString::fromLocal8Bit(CRLF));
    QString line = headers.takeFirst();

    mLine.indexIn(line);

    Http::Method method(mLine.cap(1));
    if (method == Http::Method::UNKNOWN) {
        qWarning() << "Invalid method: " << mLine.cap(1);
        return;
    }
    QString uri = mLine.cap(2);
    Http::Version version(mLine.cap(3));
    if (!version.isValid())
        qWarning() << "invalid version" << mLine.cap(3);

    Server *server = (Server *)parent();

    return server->methodGet(this, method, uri, version, headers);
}

} // namespace Http
} // namespace Freebox
