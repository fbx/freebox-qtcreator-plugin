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

#include "ssdpclient.h"
#include "ssdpconstants.h"
#include "ssdpsearch.h"
#include "ssdpmessage.h"
#include "udplistener.h"

#include <QNetworkInterface>

namespace Freebox {
namespace Ssdp {

Client::Client(QObject *parent) :
    QObject(parent),
    mRunning(false)
{
}

Client::~Client()
{
    if (isRunning())
        stop();
}

void Client::start(const QString &nt)
{
    if (isRunning())
        return;

    for (const QNetworkInterface &iface : QNetworkInterface::allInterfaces()) {
        QNetworkInterface::InterfaceFlags flags = iface.flags();
        if (!(flags & QNetworkInterface::IsUp) ||
            !(flags & QNetworkInterface::IsRunning) ||
             (flags & QNetworkInterface::IsLoopBack) ||
             (flags & QNetworkInterface::IsPointToPoint) ||
            !(flags & QNetworkInterface::CanMulticast))
            continue;

        mListeners << new UdpListener(iface);

        for (const QNetworkAddressEntry &addr : iface.addressEntries()) {
            const QHostAddress ip = addr.ip();
            if (ip.protocol() == QAbstractSocket::IPv4Protocol)
                mSearches << new Search(iface, ip, nt);
        }
    }

    for (UdpListener *l : mListeners) {
        connect(l, &UdpListener::datagramReceived,
                this, &Client::notifyMessageReceived);
    }

    for (Search *s : mSearches) {
        connect(s, &Search::datagramReceived,
                this, &Client::notifyMessageReceived);
    }

    mRunning = true;
}

void Client::stop()
{
    if (!isRunning())
        return;

    qDeleteAll(mListeners.begin(), mListeners.end());
    mListeners.clear();

    qDeleteAll(mSearches.begin(), mSearches.end());
    mSearches.clear();

    mRunning = false;
}

bool Client::isRunning() const
{
    return mRunning;
}

void Client::notifyMessageReceived(const QHostAddress &addr,
                                   const QByteArray &datagram)
{
    Message message(datagram.data());

    emit messageReceived(addr, message);
}


void Client::search()
{
    for (Search *s : mSearches) {
        s->send();
    }
}

} // namespace Ssdp
} // namespace Freebox
