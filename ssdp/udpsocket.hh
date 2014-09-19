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
#ifndef UDPSOCKET_HH_
# define UDPSOCKET_HH_

#include <QUdpSocket>

namespace Freebox {
namespace Ssdp {

class UdpSocket : public QUdpSocket
{
    Q_OBJECT

public:
    UdpSocket(QObject *parent = 0);

signals:
    void datagramReceived(const QHostAddress &addr,
                          const QByteArray &datagram);

private slots:
    void datagramRead();
};

} // namespace Ssdp
} // namespace Freebox

#endif /* !UDPSOCKET_HH_ */
