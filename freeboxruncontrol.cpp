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

#include "freeboxruncontrol.h"
#include "freeboxdevice.h"

#include <projectexplorer/project.h>
#include <projectexplorer/target.h>
#include <projectexplorer/kitinformation.h>
#include <projectexplorer/devicesupport/idevice.h>

#include <utils/url.h>

namespace Freebox {

RunControl::RunControl(ProjectExplorer::RunControl *runControl)
  : ProjectExplorer::RunWorker(runControl)
  , m_target(runControl->target())
{
    ProjectExplorer::Kit *kit = m_target->kit();
    ProjectExplorer::Project *project = m_target->project();
    ProjectExplorer::IDevice::ConstPtr device =
            ProjectExplorer::DeviceKitAspect::device(kit);
    FreeboxDevice::ConstPtr fbxDevice = device.dynamicCast<const FreeboxDevice>();

    if (fbxDevice) {
        m_address = fbxDevice->address();
        m_qmlRemote.setAddress(m_address);
    }

    if (project)
        m_server.setPath(project->projectDirectory().path());

    connect(&m_qmlRemote, &Remote::QmlRemote::started,
            this, &RunControl::emitRemoteStarted);
    connect(&m_qmlRemote, &Remote::QmlRemote::failed,
            this, &RunControl::emitRemoteFailed);
    connect(&m_stdout, &QTcpSocket::readyRead,
            this, &RunControl::readOutput);
    connect(&m_stderr, &QTcpSocket::readyRead,
            this, &RunControl::readOutput);
    connect(&m_stdout, &QTcpSocket::disconnected,
            this, &RunControl::emitRemoteStopped);
}

void RunControl::start()
{
    ProjectExplorer::Kit *kit = m_target->kit();
    ProjectExplorer::IDevice::ConstPtr device =
            ProjectExplorer::DeviceKitAspect::device(kit);
    FreeboxDevice::ConstPtr fbxDevice = device.dynamicCast<const FreeboxDevice>();

    m_server.listen(QHostAddress::Any, 8234);

    // FIXME: use entry points populated from manifest
    m_qmlRemote.start("main", m_server.serverPort(), m_debug);

    reportStarted();
}

void RunControl::stop()
{
    unlinkOutput();
    m_server.close();
    reportStopped();
}

void RunControl::emitRemoteStarted(quint16 port,
                                   quint16 out,
                                   quint16 err)
{
    QUrl serverUrl;
    serverUrl.setHost(m_address.toString());
    serverUrl.setPort(port);
    serverUrl.setScheme(Utils::urlTcpScheme());
    emit remoteStarted(serverUrl);
    linkOutput(out, err);
}

void RunControl::emitRemoteFailed(const QString &err)
{
    appendMessage(err, Utils::StdErrFormat);
}

void RunControl::emitRemoteStopped()
{
    stop();
    emit remoteStopped();
}

void RunControl::linkOutput(quint16 out, quint16 err)
{
    m_stdout.connectToHost(m_address, out);
    m_stderr.connectToHost(m_address, err);
}

void RunControl::unlinkOutput()
{
    m_stdout.disconnectFromHost();
    m_stderr.disconnectFromHost();
}

void RunControl::readOutput()
{
    QTcpSocket *output = qobject_cast<QTcpSocket *>(sender());

    if (output) {
        QByteArray data = output->readAll();

        if (output == &m_stdout)
            appendMessage(QString::fromUtf8(data), Utils::StdOutFormat);
        else if (output == &m_stderr)
            appendMessage(QString::fromUtf8(data), Utils::StdErrFormat);
    }
}

} // namespace Freebox
