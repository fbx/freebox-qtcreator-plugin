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

#include "runconfigurationfactory.hh"
#include "constants.hh"
#include "project.hh"
#include "remoterunconfiguration.hh"
#include "localrunconfiguration.hh"

#include <projectexplorer/kitinformation.h>
#include <projectexplorer/runconfiguration.h>
#include <projectexplorer/target.h>
#include <qtsupport/qtkitinformation.h>

namespace Freebox {
namespace Internal {

RunConfigurationFactory::RunConfigurationFactory(QObject *parent) :
    ProjectExplorer::IRunConfigurationFactory(parent)
{
    setObjectName(QLatin1String("FreeboxProjectRunConfigurationFactory"));
}

QList<Core::Id> RunConfigurationFactory::availableCreationIds(ProjectExplorer::Target *parent, CreationMode mode) const
{
    Q_UNUSED(mode)
    if (!canHandle(parent))
        return QList<Core::Id>();

    Project *p = qobject_cast<Project *>(parent->project());

    QList<Core::Id> list;
    if (p) {
        foreach (QString ep, p->manifest().entryPoints())
            list << Core::Id(ep.toUtf8().constData());
    }

    return list;
}

QString RunConfigurationFactory::displayNameForId(Core::Id id) const
{
    return QString::fromUtf8(id.name());
}

bool RunConfigurationFactory::canCreate(ProjectExplorer::Target *parent,
                                        const Core::Id id) const
{
    Q_UNUSED(id)

    if (!canHandle(parent))
        return false;

    Core::Id deviceTypeId =
            ProjectExplorer::DeviceTypeKitInformation::deviceTypeId(parent->kit());

    if (deviceTypeId == Freebox::Constants::FREEBOX_DEVICE_TYPE)
        return true;

    if (deviceTypeId == ProjectExplorer::Constants::DESKTOP_DEVICE_TYPE) {
        QtSupport::BaseQtVersion *version
                = QtSupport::QtKitInformation::qtVersion(parent->kit());
        return version && version->qtVersion() >= QtSupport::QtVersionNumber(5, 0, 0);
    }

    return false;
}

ProjectExplorer::RunConfiguration *RunConfigurationFactory::doCreate(ProjectExplorer::Target *parent, Core::Id id)
{
    Core::Id deviceTypeId =
            ProjectExplorer::DeviceTypeKitInformation::deviceTypeId(parent->kit());

    if (deviceTypeId == Freebox::Constants::FREEBOX_DEVICE_TYPE)
        return new RemoteRunConfiguration(parent, id);

    if (deviceTypeId == ProjectExplorer::Constants::DESKTOP_DEVICE_TYPE)
        return new LocalRunConfiguration(parent, id);

    QTC_ASSERT(false, return 0);

    return 0;
}

bool RunConfigurationFactory::canRestore(ProjectExplorer::Target *parent, const QVariantMap &map) const
{
    return parent && canCreate(parent, ProjectExplorer::idFromMap(map));
}

ProjectExplorer::RunConfiguration *RunConfigurationFactory::doRestore(ProjectExplorer::Target *parent,
                                                                      const QVariantMap &map)
{
    return doCreate(parent, ProjectExplorer::idFromMap(map));
}

bool RunConfigurationFactory::canClone(ProjectExplorer::Target *parent, ProjectExplorer::RunConfiguration *source) const
{
    return canCreate(parent, source->id());
}

ProjectExplorer::RunConfiguration *RunConfigurationFactory::clone(ProjectExplorer::Target *parent,
                                                                  ProjectExplorer::RunConfiguration *source)
{
    if (!canClone(parent, source))
        return 0;

    RemoteRunConfiguration *fbx = qobject_cast<RemoteRunConfiguration *>(source);
    if (fbx)
        return new RemoteRunConfiguration(parent, fbx);

    LocalRunConfiguration *local = qobject_cast<LocalRunConfiguration *>(source);
    if (local)
        return new LocalRunConfiguration(parent, local);

    QTC_ASSERT(false, return 0);
    return 0;
}

bool RunConfigurationFactory::canHandle(ProjectExplorer::Target *parent) const
{
    if (!parent->project()->supportsKit(parent->kit()))
        return false;
    if (!qobject_cast<Project *>(parent->project()))
        return false;

    return true;
}

} // namespace Internal
} // namespace Freebox
