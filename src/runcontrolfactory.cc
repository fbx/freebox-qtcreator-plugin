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
#include <analyzerbase/analyzerstartparameters.h>
#include <analyzerbase/analyzermanager.h>
#include <analyzerbase/analyzerruncontrol.h>
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
                               ProjectExplorer::RunMode mode) const
{
    Q_UNUSED(mode);

    RemoteRunConfiguration *remote = qobject_cast<RemoteRunConfiguration *>(rc);
    if (remote)
        return true;

    return false;
}

ProjectExplorer::RunControl *
RunControlFactory::create(ProjectExplorer::RunConfiguration *runConfiguration,
                          ProjectExplorer::RunMode mode,
                          QString *errorMessage)
{
    Q_UNUSED(errorMessage);

    ProjectExplorer::Kit *kit = runConfiguration->target()->kit();
    ProjectExplorer::IDevice::ConstPtr device =
            ProjectExplorer::DeviceKitInformation::device(kit);
    FreeboxDevice::ConstPtr fbxDevice = device.dynamicCast<const FreeboxDevice>();

    QTC_ASSERT(fbxDevice, return 0);

    switch (mode) {
    case ProjectExplorer::NormalRunMode:
        return new RunControl(runConfiguration, mode);

    case ProjectExplorer::DebugRunMode: {
        Debugger::DebuggerStartParameters params;

        params.masterEngineType = Debugger::QmlEngineType;
        params.startMode = Debugger::AttachToRemoteServer;
        params.languages = Debugger::QmlLanguage;
        params.qmlServerAddress = fbxDevice->address().toString();
        params.qmlServerPort = 0;
        params.remoteSetupNeeded = true;

        const Project *project = qobject_cast<Project *>(runConfiguration->target()->project());
        QTC_ASSERT(project, return 0);

        if (project) {
            params.projectSourceDirectory = project->projectDir().canonicalPath();
            params.projectSourceFiles =
                    project->files(ProjectExplorer::Project::ExcludeGeneratedFiles);
        }

        Debugger::DebuggerRunControl * const runControl =
                Debugger::DebuggerRunControlFactory::doCreate(params,
                                                              runConfiguration,
                                                              errorMessage);
        if (!runControl)
            return 0;

        DebuggerRunControl *debugger = new DebuggerRunControl(runControl);
        return debugger->runControl();
    }

    case ProjectExplorer::QmlProfilerRunMode: {
        Analyzer::AnalyzerStartParameters params;

        params.startMode = Analyzer::StartLocal;
        params.runMode = mode;
        params.analyzerHost = fbxDevice->address().toString();
        params.analyzerPort = 0;

        Analyzer::AnalyzerRunControl * const runControl =
                Analyzer::AnalyzerManager::createRunControl(params,
                                                            runConfiguration);
        if (!runControl)
            return 0;

        AnalyzerRunControl *analyzer = new AnalyzerRunControl(runControl);
        return analyzer->runControl();
    }

    default:
        qWarning("Unsupported run mode");
    }

    return 0;
}

} // namespace Freebox
