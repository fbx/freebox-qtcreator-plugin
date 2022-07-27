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

#include "http/server.h"
#include "remote/remoteqml.h"

#include <projectexplorer/runcontrol.h>

#include <QNetworkInterface>
#include <QHostAddress>
#include <QTcpSocket>

namespace Freebox {

class RunControl : public ProjectExplorer::RunWorker
{
    Q_OBJECT

public:
    RunControl(ProjectExplorer::RunControl *runControl);

    void start() override;
    void stop() override;

signals:
    void remoteStarted(quint16 port);
    void remoteStopped();

private slots:
    void emitRemoteStarted(quint16 port,
                           quint16 out,
                           quint16 err);
    void emitRemoteFailed(const QString &err);
    void emitRemoteStopped();

    void linkOutput(quint16 out, quint16 err);
    void unlinkOutput();
    void readOutput();

protected:
    bool m_debug = false;

private:
    QPointer<ProjectExplorer::Target> m_target;
    QNetworkInterface m_iface;
    QHostAddress m_address;
    Http::Server m_server;
    Remote::QmlRemote m_qmlRemote;

    QTcpSocket m_stdout;
    QTcpSocket m_stderr;
};

} // namespace Freebox

