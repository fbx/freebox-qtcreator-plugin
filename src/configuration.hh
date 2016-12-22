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
#pragma once

#include <QObject>
#include <QHash>
#include <QNetworkInterface>
#include <QHostAddress>

namespace Freebox {

namespace Ssdp {
class Client;
class Message;
}

class FreeboxDevice;

namespace Internal {

class FreeboxPlugin;

class FreeboxConfiguration : public QObject
{
    friend class FreeboxPlugin;

    Q_OBJECT

public slots:
    void updateKits();
    void updateDevices();
    void filterMessage(const QHostAddress &addr,
                       const Ssdp::Message &message);
    void freeboxAdd(const QHostAddress &addr,
                    const QString &usn);
    void freeboxDel(const QString &usn);

signals:
    void freeboxAlive(const QHostAddress &addr,
                      const QString &usn);
    void freeboxByebye(const QString &usn);

private:
    FreeboxConfiguration(QObject *parent);

    Ssdp::Client *mSSDPClient;
    QHash<QString, QString *> mFreebox;
};

} // namespace Internal
} // namespace Freebox

