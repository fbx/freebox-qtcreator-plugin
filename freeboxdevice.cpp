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

#include "freeboxdevice.h"
#include "freeboxconstants.h"
#include "freeboxsignaloperation.h"

#include <projectexplorer/runconfiguration.h>

#include <utils/url.h>

#include <QCoreApplication>

using namespace ProjectExplorer;

namespace Freebox {

FreeboxDevice::FreeboxDevice()
{
    setupId(IDevice::AutoDetected, Constants::FREEBOX_DEVICE_ID);
    setType(Constants::FREEBOX_DEVICE_TYPE);
    setDisplayType("Freebox");
    setDisplayName("Freebox Device");
    setMachineType(IDevice::Hardware);
    setOsType(Utils::OsType::OsTypeOtherUnix);
    setDeviceState(IDevice::DeviceStateUnknown);
}

FreeboxDevice::FreeboxDevice(Utils::Id id, const QHostAddress &addr) :
    mAddress(addr)
{
    setupId(IDevice::AutoDetected, id);
    setType(Constants::FREEBOX_DEVICE_TYPE);
    setDisplayType("Freebox");
    setDisplayName(QLatin1String("Freebox(") + addr.toString() + QLatin1String(")"));
    setMachineType(IDevice::Hardware);
    setOsType(Utils::OsType::OsTypeOtherUnix);
    setDeviceState(IDevice::DeviceStateUnknown);
}

IDevice::Ptr FreeboxDevice::create()
{
    return IDevice::Ptr(new FreeboxDevice);
}

ProjectExplorer::IDevice::DeviceInfo FreeboxDevice::deviceInformation() const
{
    return DeviceInfo();
}

ProjectExplorer::IDeviceWidget *FreeboxDevice::createWidget()
{
    return 0;
    // DesktopDeviceConfigurationWidget currently has just one editable field viz. free ports.
    // Querying for an available port is quite straightforward. Having a field for the port
    // range can be confusing to the user. Hence, disabling the widget for now.
}

bool FreeboxDevice::canAutoDetectPorts() const
{
    return true;
}

DeviceProcessSignalOperation::Ptr FreeboxDevice::signalOperation() const
{
    return DeviceProcessSignalOperation::Ptr(new FreeboxSignalOperation());
}

QUrl FreeboxDevice::toolControlChannel(const ControlChannelHint &) const
{
    QUrl url;
    url.setScheme(Utils::urlTcpScheme());
    url.setHost(mAddress.toString());
    return url;
}

const QHostAddress &FreeboxDevice::address() const
{
    return mAddress;
}

void FreeboxDevice::fromMap(const QVariantMap &map)
{
    IDevice::fromMap(map);
    mAddress = QHostAddress(map.value(QLatin1String("address")).toString());
}

QVariantMap FreeboxDevice::toMap() const
{
    QVariantMap res = IDevice::toMap();
    res.insert(QLatin1String("address"), mAddress.toString());
    return res;
}

FreeboxDeviceFactory::FreeboxDeviceFactory()
    : IDeviceFactory(Constants::FREEBOX_DEVICE_TYPE)
{
    setDisplayName("Freebox Device");
    setCombinedIcon(":/freebox/images/tvdevicesmall.png",
                    ":/freebox/images/tvdevice.png");
    setConstructionFunction(&FreeboxDevice::create);
}

} // namespace Freebox
