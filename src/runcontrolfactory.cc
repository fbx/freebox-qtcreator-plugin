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
#include <QDebug>

#include <debugger/debuggerstartparameters.h>
#include <debugger/debuggerruncontrol.h>
#include <debugger/debuggerengine.h>
#include <debugger/analyzer/analyzerstartparameters.h>
#include <debugger/analyzer/analyzermanager.h>
#include <debugger/analyzer/analyzerruncontrol.h>
#include <projectexplorer/project.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/target.h>
#include <projectexplorer/kitinformation.h>

#include "constants.hh"
#include "remoterunconfiguration.hh"
#include "localrunconfiguration.hh"
#include "runcontrolfactory.hh"
#include "runcontrol.hh"
#include "project.hh"
#include "debugger.hh"
#include "analyzer.hh"
#include "device.hh"

namespace Freebox {

RunControlFactory::RunControlFactory(QObject *parent) :
    IRunControlFactory(parent)
{
}

bool RunControlFactory::canRun(ProjectExplorer::RunConfiguration *rc,
                               Core::Id mode) const
{
    Q_UNUSED(mode);

    RemoteRunConfiguration *remote = qobject_cast<RemoteRunConfiguration *>(rc);
    if (remote)
        return true;

    return false;
}

ProjectExplorer::RunControl *
RunControlFactory::create(ProjectExplorer::RunConfiguration *runConfiguration,
                          Core::Id mode,
                          QString *errorMessage)
{
    Q_UNUSED(errorMessage);

    ProjectExplorer::Kit *kit = runConfiguration->target()->kit();
    ProjectExplorer::IDevice::ConstPtr device =
            ProjectExplorer::DeviceKitInformation::device(kit);
    FreeboxDevice::ConstPtr fbxDevice = device.dynamicCast<const FreeboxDevice>();

    QTC_ASSERT(fbxDevice, return 0);

    if (mode == ProjectExplorer::Constants::NORMAL_RUN_MODE) {
        return new RunControl(runConfiguration, mode);

    } else if (mode == ProjectExplorer::Constants::DEBUG_RUN_MODE) {
        Debugger::DebuggerStartParameters params;

        params.startMode = Debugger::AttachToRemoteServer;
        params.qmlServerAddress = fbxDevice->address().toString();
        params.qmlServerPort = 0;

        const Project *project = qobject_cast<Project *>(runConfiguration->target()->project());
        QTC_ASSERT(project, return 0);

        Debugger::DebuggerRunControl * const runControl = createDebuggerRunControl(params, runConfiguration, errorMessage);
        if (!runControl)
            return 0;

        DebuggerRunControl *debugger = new DebuggerRunControl(runControl);
        return debugger->runControl();

    } else if (mode == ProjectExplorer::Constants::QML_PROFILER_RUN_MODE) {
        Debugger::AnalyzerRunControl * const runControl =
                Debugger::createAnalyzerRunControl(runConfiguration, mode);
        if (!runControl)
            return 0;

        AnalyzerRunControl *analyzer = new AnalyzerRunControl(runControl);
        return analyzer->runControl();

    } else {
        qWarning("Unsupported run mode");
    }

    return 0;
}

} // namespace Freebox
