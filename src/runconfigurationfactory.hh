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
#ifndef FREEBOXRUNCONFIGURATIONFACTORY_HH_
# define FREEBOXRUNCONFIGURATIONFACTORY_HH_

#include <projectexplorer/runconfiguration.h>

namespace Freebox {
namespace Internal {

class RunConfigurationFactory : public ProjectExplorer::IRunConfigurationFactory
{
    Q_OBJECT

public:
    explicit RunConfigurationFactory(QObject *parent = 0);
    ~RunConfigurationFactory();

    QList<Core::Id> availableCreationIds(ProjectExplorer::Target *parent, CreationMode mode = UserCreate) const;
    QString displayNameForId(Core::Id id) const;

    bool canCreate(ProjectExplorer::Target *parent, const Core::Id id) const;
    bool canRestore(ProjectExplorer::Target *parent, const QVariantMap &map) const;
    bool canClone(ProjectExplorer::Target *parent, ProjectExplorer::RunConfiguration *source) const;
    ProjectExplorer::RunConfiguration *clone(ProjectExplorer::Target *parent, ProjectExplorer::RunConfiguration *source);

private:
    bool canHandle(ProjectExplorer::Target *parent) const;

    ProjectExplorer::RunConfiguration *doCreate(ProjectExplorer::Target *parent, Core::Id id);
    ProjectExplorer::RunConfiguration *doRestore(ProjectExplorer::Target *parent,
                                                 const QVariantMap &map);
};

} // namespace Internal
} // namespace Freebox

#endif /* !FREEBOXRUNCONFIGURATIONFACTORY_HH_ */
