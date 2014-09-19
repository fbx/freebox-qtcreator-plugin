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

#include "constants.hh"
#include "manager.hh"
#include "project.hh"

namespace Freebox {
namespace Internal {

QString Manager::mimeType() const
{
    return QLatin1String(Constants::FBXPROJECT_MIMETYPE);
}

ProjectExplorer::Project *Manager::openProject(const QString &fileName,
                                               QString *errorString)
{
    if (!QFileInfo(fileName).isFile()) {
        if (errorString)
            *errorString = tr("Failed opening project '%1': Project is not a file")
                .arg(fileName);
        return 0;
    }

    return new Project(this, fileName);
}

void Manager::registerProject(Project *project)
{
    m_projects.append(project);
}

void Manager::unregisterProject(Project *project)
{
    m_projects.removeAll(project);
}

void Manager::notifyChanged(const QString &fileName)
{
    foreach (Project *project, m_projects) {
        if (fileName == project->filesFileName())
            project->refresh(Project::RefreshFiles);
    }
}

} // namespace Internal
} // namespace Freebox
