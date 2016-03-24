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
#ifndef MANAGER_HH_
# define MANAGER_HH_

#include <projectexplorer/iprojectmanager.h>

namespace Freebox {

class Project;

namespace Internal {

class Manager : public ProjectExplorer::IProjectManager
{
    Q_OBJECT

public:
    Manager();

    QString mimeType() const override;
    ProjectExplorer::Project *openProject(const QString &filename, QString *errorString) override;

    void notifyChanged(const QString &fileName);

    void registerProject(Project *project);
    void unregisterProject(Project *project);

private:
    QList<Project *> m_projects;

};
} // namespace Internal
} // namespace Freebox

#endif /* !MANAGER_HH_ */
