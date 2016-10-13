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

#include "debugger.hh"
#include "project.hh"

#include <projectexplorer/runconfiguration.h>
#include <projectexplorer/target.h>

#include <debugger/debuggerruncontrol.h>
#include <debugger/debuggerstartparameters.h>

#include <utils/port.h>

namespace Freebox {

DebuggerRunControl::DebuggerRunControl(Debugger::DebuggerRunControl *runControl) :
    RunControl(runControl->runConfiguration(), runControl->runMode()),
    mRunControl(runControl)
{
    mDebug = true;

    connect(mRunControl, &Debugger::DebuggerRunControl::finished,
            this, &DebuggerRunControl::stop);

    connect(mRunControl, &Debugger::DebuggerRunControl::requestRemoteSetup,
            this, &DebuggerRunControl::start);

    connect(this, &DebuggerRunControl::remoteStarted,
        [this](quint16 port) {
            Debugger::RemoteSetupResult result;
            result.success = true;
            result.qmlServerPort = Utils::Port(port);
            mRunControl->notifyEngineRemoteSetupFinished(result);
        });

    connect(this, &DebuggerRunControl::remoteStopped,
        [this]() {
            mRunControl->notifyInferiorExited();
        });
}

void DebuggerRunControl::appendMessage(const QString &msg, Utils::OutputFormat format)
{
    mRunControl->appendMessage(msg, format);
}

} // namespace Freebox
