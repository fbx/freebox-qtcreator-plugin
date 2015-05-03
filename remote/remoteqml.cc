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

#include "remoteconstants.hh"
#include "remoteqml.hh"

#include "../util/jsonrpc.hh"

#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkInterface>
#include <QNetworkAddressEntry>
#include <QHostAddress>

namespace Freebox {
namespace Remote {

static QString makeUrl(const QHostAddress &addr)
{
    return QString::fromLocal8Bit(Constants::URL).arg(addr.toString());
}

QmlRemote::QmlRemote(QObject *parent) :
    QObject(parent),
    mAddress(),
    mCurrent(0),
    mStarting(false)
{
}

QmlRemote::~QmlRemote()
{
}

void QmlRemote::setAddress(const QHostAddress &addr)
{
    mAddress = addr;
}

const QHostAddress &QmlRemote::address() const
{
    return mAddress;
}

void QmlRemote::startError(const QJsonObject &err)
{
    if (sender() != mCurrent)
        return;

    qWarning() << err;
    emit failed(err[QStringLiteral("message")].toString());
    mCurrent = 0;
}

void QmlRemote::startDone(const QJsonValue &rep)
{
    if (sender() != mCurrent)
        return;

    QJsonObject v = rep.toObject();

    emit started(v[QStringLiteral("qml_port")].toInt(),
                 v[QStringLiteral("stdout_port")].toInt(),
                 v[QStringLiteral("stderr_port")].toInt());
    mCurrent = 0;
}

void QmlRemote::start(const QString &entry, quint16 port, bool wait)
{
    if (mStarting)
        return;

    if (mAddress.isNull()) {
        qWarning("remote address is not set");
        return;
    }

    QTcpSocket socket;
    socket.connectToHost(mAddress, 80);
    if (!socket.waitForConnected(3000)) {
        qWarning() << "fail to connect to" << mAddress.toString();
        return;
    }
    QHostAddress me = socket.localAddress();
    socket.close();

    QJsonObject arg;
    arg[QStringLiteral("manifest_url")] = QString::fromLocal8Bit(Constants::MANIFEST_URL)
        .arg(me.toString())
        .arg(QString::number(port));
    arg[QStringLiteral("entry_point")] = entry;
    arg[QStringLiteral("wait")] = wait;

    QString url = makeUrl(mAddress);
    mCurrent = new Util::JsonRpcRequest(url, QStringLiteral("debug_qml_app"), arg, this);

    connect(mCurrent, SIGNAL(error(const QJsonObject &)), SLOT(startError(const QJsonObject &)));
    connect(mCurrent, SIGNAL(done(const QJsonValue &)), SLOT(startDone(const QJsonValue &)));
}

} // namespace Remote
} // namespace Freebox
