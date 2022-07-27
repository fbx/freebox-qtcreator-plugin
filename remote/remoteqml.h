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
#include <QHostAddress>
#include <QJsonObject>
#include <QJsonValue>

namespace Freebox {

namespace Util { class JsonRpcRequest; }

namespace Remote {

class QmlRemote : public QObject
{
    Q_OBJECT
public:
    explicit QmlRemote(QObject *parent = 0);
    virtual ~QmlRemote();

    void start(const QString &entry, quint16 port, bool wait = false);

    void setAddress(const QHostAddress &addr);
    const QHostAddress &address() const;

signals:
    void started(quint16 port,
                 quint16 out,
                 quint16 err);
    void failed(const QString &err);
    void stopped();

private slots:
    void startError(const QJsonObject &);
    void startDone(const QJsonValue &);

private:
    QHostAddress mAddress;
    Util::JsonRpcRequest *mCurrent;
    bool mStarting;
};

} // namespace Remote
} // namespace Freebox

