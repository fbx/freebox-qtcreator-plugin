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
#include <QRegExp>

#include "ssdpconstants.hh"
#include "udplistener.hh"
#include "ssdpclient.hh"
#include "ssdpsearch.hh"
#include "ssdpmessage.hh"

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
    QList<const char *> interfaces;
    if (isRunning())
        return;


    foreach(QNetworkInterface iface, QNetworkInterface::allInterfaces()) {
        mListeners << new UdpListener(iface);
        mSearches << new Search(iface, nt);
    }

    foreach(UdpListener *l, mListeners) {
        connect(l, SIGNAL(datagramReceived(const QHostAddress &,
                                           const QByteArray &)),
                SLOT(notifyMessageReceived(const QHostAddress &,
                                           const QByteArray &)));
    }

    foreach(Search *s, mSearches) {
        connect(s, SIGNAL(datagramReceived(const QHostAddress &,
                                           const QByteArray &)),
                SLOT(notifyMessageReceived(const QHostAddress &,
                                           const QByteArray &)));
    }

    mRunning = true;
}

void Client::stop() {
    if (!isRunning())
        return;

    foreach(UdpListener *l, mListeners) {
        mListeners.removeAll(l);
        delete l;
    }

    foreach(Search *s, mSearches) {
        mSearches.removeAll(s);
        delete s;
    }
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
    foreach(Search *s, mSearches) {
        s->send();
    }
}

} // namespace Ssdp
} // namespace Freebox
