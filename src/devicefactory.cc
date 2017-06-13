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

#include "devicefactory.hh"
#include "device.hh"
#include "constants.hh"

#include <utils/icon.h>

#include <QIcon>

namespace Freebox {
namespace Internal {

DeviceFactory::DeviceFactory()
{
    setObjectName(QStringLiteral("FreeboxDeviceFactory"));
}

QString DeviceFactory::displayNameForId(Core::Id type) const
{
    if (type == Constants::FREEBOX_DEVICE_TYPE)
        return tr("Freebox");
    return QString();
}

QList<Core::Id> DeviceFactory::availableCreationIds() const
{
    return QList<Core::Id>() << Core::Id(Constants::FREEBOX_DEVICE_TYPE);
}

QIcon DeviceFactory::iconForId(Core::Id type) const
{
    Q_UNUSED(type)
    using namespace Utils;
    static const QIcon icon =
            Icon::combinedIcon({Icon({{":/freebox/images/tvdevicesmall.png",
                                       Theme::PanelTextColorDark}}, Icon::Tint),
                                Icon({{":/freebox/images/tvdevice.png",
                                       Theme::IconsBaseColor}})});
    return icon;
}

bool DeviceFactory::canCreate() const
{
    return false;
}

ProjectExplorer::IDevice::Ptr DeviceFactory::create(Core::Id id) const
{
    Q_UNUSED(id);
    return ProjectExplorer::IDevice::Ptr();
}

bool DeviceFactory::canRestore(const QVariantMap &map) const
{
    Q_UNUSED(map);
    return false;
}

ProjectExplorer::IDevice::Ptr DeviceFactory::restore(const QVariantMap &map) const
{
    Q_UNUSED(map);
    return ProjectExplorer::IDevice::Ptr();
}

} // namespace Internal
} // namespace Freebox
