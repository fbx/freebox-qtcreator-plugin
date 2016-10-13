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

#include "analyzer.hh"
#include "project.hh"
#include "configuration.hh"

#include <projectexplorer/project.h>
#include <projectexplorer/target.h>

#include <debugger/analyzer/analyzerruncontrol.h>

#include <utils/port.h>

namespace Freebox {

AnalyzerRunControl::AnalyzerRunControl(Debugger::AnalyzerRunControl *runControl) :
    RunControl(runControl->runConfiguration(), runControl->runMode()),
    mRunControl(runControl)
{
    mDebug = true;

    connect(mRunControl, &Debugger::AnalyzerRunControl::starting,
            this, &AnalyzerRunControl::start);
    connect(mRunControl, &Debugger::AnalyzerRunControl::finished,
            this, &AnalyzerRunControl::stop);

    connect(this, &AnalyzerRunControl::remoteStarted,
        [this](quint16 port) {
            mRunControl->notifyRemoteSetupDone(Utils::Port(port));
        });

    connect(this, &AnalyzerRunControl::remoteStopped,
        [this]() {
            mRunControl->notifyRemoteFinished();
        });
}

void AnalyzerRunControl::appendMessage(const QString &msg, Utils::OutputFormat format)
{
    mRunControl->appendMessage(msg, format);
}

} // namespace Freebox
