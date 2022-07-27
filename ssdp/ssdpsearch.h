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

#include <QUdpSocket>
#include <QNetworkInterface>
#include <QTimer>

#include "udpsocket.h"

namespace Freebox {
namespace Ssdp {

class Search : public UdpSocket
{
public:
    Search(const QNetworkInterface &iface,
           const QHostAddress &addr,
           const QString &st,
           QObject *parent = 0);

    void send();

private:
    void handleTimeout();
    void sendPacket();

    QNetworkInterface mIface;
    QString mSt;
    QTimer m_timeout;
    int m_count = 0;
};

} // namespace Ssdp
} // namespace Freebox
