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

#include "device.hh"
#include "constants.hh"
#include "processsignaloperation.hh"

#include <QCoreApplication>

namespace Freebox {

FreeboxDevice::FreeboxDevice(Core::Id id, const QHostAddress &addr) :
    ProjectExplorer::IDevice(Core::Id(Constants::FREEBOX_DEVICE_TYPE),
                             ProjectExplorer::IDevice::AutoDetected,
                             ProjectExplorer::IDevice::Hardware,
                             Core::Id(id)),
    mAddress(addr)
{
    setDisplayName(QStringLiteral("Freebox(") + addr.toString() + QStringLiteral(")"));
    setDeviceState(IDevice::DeviceStateUnknown);
}

FreeboxDevice::FreeboxDevice(const FreeboxDevice &other) :
    ProjectExplorer::IDevice(other),
    mAddress(other.mAddress)
{
}

FreeboxDevice::~FreeboxDevice()
{
}

ProjectExplorer::IDevice::DeviceInfo FreeboxDevice::deviceInformation() const
{
    return DeviceInfo();
}

QString FreeboxDevice::displayType() const
{
    return QCoreApplication::translate("ProjectExplorer::FreeboxDevice", "Freebox");
}

ProjectExplorer::IDeviceWidget *FreeboxDevice::createWidget()
{
    return 0;
    // DesktopDeviceConfigurationWidget currently has just one editable field viz. free ports.
    // Querying for an available port is quite straightforward. Having a field for the port
    // range can be confusing to the user. Hence, disabling the widget for now.
}

QList<Core::Id> FreeboxDevice::actionIds() const
{
    return QList<Core::Id>();
}

QString FreeboxDevice::displayNameForActionId(Core::Id actionId) const
{
    Q_UNUSED(actionId);
    return QString();
}

void FreeboxDevice::executeAction(Core::Id actionId, QWidget *parent)
{
    Q_UNUSED(actionId);
    Q_UNUSED(parent);
}

bool FreeboxDevice::canAutoDetectPorts() const
{
    return true;
}

bool FreeboxDevice::canCreateProcessModel() const
{
    return false;
}

ProjectExplorer::DeviceProcessSignalOperation::Ptr
FreeboxDevice::signalOperation() const
{
    return ProjectExplorer::DeviceProcessSignalOperation::Ptr(new FreeboxProcessSignalOperation());
}

ProjectExplorer::IDevice::Ptr FreeboxDevice::clone() const
{
    return Ptr(new FreeboxDevice(*this));
}

QString FreeboxDevice::qmlProfilerHost() const
{
    return mAddress.toString();
}

const QHostAddress &FreeboxDevice::address() const
{
    return mAddress;
}

} // namespace Freebox
