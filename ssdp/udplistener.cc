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

#include "udplistener.hh"
#include "ssdpconstants.hh"

#include <QNetworkInterface>

namespace Freebox {
namespace Ssdp {

UdpListener::UdpListener(const QNetworkInterface &iface,
                         QObject *parent) :
    UdpSocket(parent),
    mIface(iface)
{
    connect(this, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            SLOT(join(QAbstractSocket::SocketState)));

    if (!bind(QHostAddress(QLatin1String(Constants::ADDR4)), Constants::PORT,
              QAbstractSocket::ShareAddress |
              QAbstractSocket::ReuseAddressHint)) {
        qWarning() << "failed to bind SSDP socket on iface " << iface << ":" << errorString();
        return;
    }
}

UdpListener::~UdpListener()
{
}

void UdpListener::join(QAbstractSocket::SocketState state)
{
    if (state != QAbstractSocket::BoundState)
        return;

    setMulticastInterface(mIface);
    if (!joinMulticastGroup(QHostAddress(QLatin1String(Constants::ADDR4)), mIface))
        qWarning() << "failed to join SSDP multicast address on iface " << mIface << ":" << errorString();
}

const QNetworkInterface &UdpListener::interface() const
{
    return mIface;
}

} // namespace Ssdp
} // namespace Freebox
