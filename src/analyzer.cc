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

namespace Freebox {

AnalyzerRunControl::AnalyzerRunControl(Analyzer::AnalyzerRunControl *runControl) :
    RunControl(runControl->runConfiguration(), runControl->runMode()),
    mRunControl(runControl)
{
    mDebug = true;

    connect(mRunControl, SIGNAL(starting(const Analyzer::AnalyzerRunControl*)),
            SLOT(remoteSetup()));
    connect(mRunControl, SIGNAL(finished()), SLOT(stop()));
    connect(this, SIGNAL(remoteStopped()), SLOT(remoteIsStopped()));
    connect(this, SIGNAL(appendMessage(ProjectExplorer::RunControl *,
                                       const QString &,
                                       Utils::OutputFormat)),
            SLOT(redirectMessage(ProjectExplorer::RunControl *,
                                 const QString &,
                                 Utils::OutputFormat)));
}

void AnalyzerRunControl::redirectMessage(ProjectExplorer::RunControl *rc,
                                         const QString &msg,
                                         Utils::OutputFormat format)
{
    Q_UNUSED(rc);
    mRunControl->logApplicationMessage(msg, format);
}

Analyzer::AnalyzerRunControl *AnalyzerRunControl::runControl() const
{
    return mRunControl;
}

void AnalyzerRunControl::remoteSetup()
{
    start();
}

void AnalyzerRunControl::remoteIsStopped()
{
    mRunControl->notifyRemoteFinished();
}

} // namespace Freebox
