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
#ifndef DEVICEFACTORY_HH_
# define DEVICEFACTORY_HH_

#include <projectexplorer/devicesupport/idevicefactory.h>

namespace Freebox {
namespace Internal {

class DeviceFactory : public ProjectExplorer::IDeviceFactory
{
    Q_OBJECT;

public:
    DeviceFactory();

    QString displayNameForId(Core::Id type) const;
    virtual QList<Core::Id> availableCreationIds() const;
    virtual ProjectExplorer::IDevice::Ptr create(Core::Id id) const;
    virtual bool canCreate() const;
    virtual bool canRestore(const QVariantMap &map) const;
    virtual ProjectExplorer::IDevice::Ptr restore(const QVariantMap &map) const;
};

} // namespace Internal
} // namespace Freebox

#endif /* !DEVICEFACTORY_HH_ */
