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
#include <QNetworkInterface>

#include "ssdpconstants.hh"
#include "udpsocket.hh"
#include "ssdpsearch.hh"

namespace Freebox {
namespace Ssdp {

Search::Search(const QNetworkInterface &iface,
               const QHostAddress &addr,
               const QString &st,
               QObject *parent) :
    UdpSocket(parent),
    mIface(iface),
    mSt(st)
{
    if (!bind(addr)) {
        qWarning() << "failed bind for SSDP search:" << errorString();
        return;
    }

    setMulticastInterface(iface);
}

void Search::send()
{
    QString message;

    if (!mIface.isValid()) {
        qWarning("invalid interface");
        return;
    }

    if (mSt.isEmpty()) {
        qWarning("ST field is empty");
        return;
    }

    message = QStringLiteral("M-SEARCH * HTTP/1.1\r\n");
    message += QString::fromAscii("HOST: %1:%2\r\n")
        .arg(QString::fromAscii(Constants::ADDR4))
        .arg(QString::number(Constants::PORT));
    message += QStringLiteral("MAN: \"ssdp:discover\"\r\n");
    message += QStringLiteral("MX: 1\r\n");
    message += QString::fromAscii("ST: %1\r\n").arg(mSt);
    message += QStringLiteral("\r\n");

    writeDatagram(message.toLatin1().constData(), message.size(),
                  QHostAddress(QString::fromAscii(Constants::ADDR4)), Constants::PORT);
}

} // namespace Ssdp
} // namespace Freebox
