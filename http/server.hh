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
#ifndef SERVER_HH_
# define SERVER_HH_

#include <QTcpServer>
#include <QFile>
#include <QDir>

#include "http.hh"

namespace Freebox {
namespace Http {

class Method;
class Version;

class Client;

class Server : public QTcpServer
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = 0);

    void incomingConnection(qintptr handle);
    void setPath(QString path) { mPath.setPath(path); }

private slots:
    void readClient();
    void disconnectClient();

private:
    friend class Client;

    void methodGet(Client *client,
                   Method &method,
                   QString &uri,
                   Version &version,
                   QStringList &headers);
    void replyError(QTcpSocket *socket,
                    const Version &version,
                    const IResponse &response);
    void reply(QTcpSocket *socket,
               const Version &version,
               const IResponse &response);

    void replyHeader(QTcpSocket *socket,
                     QString name,
                     unsigned int value);
    void replyHeader(QTcpSocket *socket,
                     QString name,
                     QString value);
    void replyHeaderEnd(QTcpSocket *socket);

    QDir mPath;
};

} // namespace Http
} // namespace Freebox


#endif /* !SERVER_HH_ */
