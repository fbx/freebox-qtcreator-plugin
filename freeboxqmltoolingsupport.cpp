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

  Copyright (c) 2022, Freebox SAS, See AUTHORS for details.
*/

#include "freeboxqmltoolingsupport.h"
#include "freeboxruncontrol.h"

#include <qmldebug/qmldebugcommandlinearguments.h>

using namespace ProjectExplorer;

namespace Freebox {

FreeboxQmlToolingSupport::FreeboxQmlToolingSupport(ProjectExplorer::RunControl *runControl)
    : RunWorker(runControl)
{
    setId("FreeboxQmlToolingSupport");

    auto runner = new RunControl(runControl);
    addStartDependency(runner);

    auto worker = runControl->createWorker(QmlDebug::runnerIdForRunMode(runControl->runMode()));
    worker->addStartDependency(this);

    connect(runner, &RunControl::remoteStarted, this, [this, worker](const QUrl &server) {
        worker->recordData("QmlServerUrl", server);
        reportStarted();
    });
}

void FreeboxQmlToolingSupport::start()
{
}

void FreeboxQmlToolingSupport::stop()
{
    reportStopped();
}

} // namespace Freebox
