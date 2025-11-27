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

#include <utils/environment.h>
#include <utils/url.h>

#include <QCoreApplication>

using namespace ProjectExplorer;

namespace Freebox::Internal {

FreeboxDevice::FreeboxDevice()
{
    setFreePorts(Utils::PortList::fromString("32950-32990"));

    setupId(IDevice::AutoDetected, Constants::FREEBOX_DEVICE_ID);
    setType(Constants::FREEBOX_DEVICE_TYPE);
    setDefaultDisplayName("Freebox");
    setDisplayName("Freebox Device");
    setMachineType(IDevice::Hardware);
    setOsType(Utils::OsType::OsTypeOtherUnix);
    setDeviceState(IDevice::DeviceDisconnected);
}

void FreeboxDevice::configure(const QString &usn, const QHostAddress &addr)
{
    mAddress = addr;
    setupId(IDevice::AutoDetected, Utils::Id::fromString(usn));
    setDisplayType("Freebox");
    setDisplayName(QLatin1String("Freebox(") + addr.toString() + QLatin1String(")"));
}

Utils::Result<Utils::Environment> FreeboxDevice::systemEnvironmentWithError() const
{
    // The Freebox is reached over a QML remote channel and exposes no usable
    // shell environment to Qt Creator. Return an empty (but valid) environment
    // so that the generic kit/run machinery does not trip the fileAccess() assert.
    return Utils::Environment{};
}

ProjectExplorer::IDevice::DeviceInfo FreeboxDevice::deviceInformation() const
{
    return DeviceInfo();
}

Tasking::ExecutableItem FreeboxDevice::portsGatheringRecipe(
    const Tasking::Storage<Utils::PortsOutputData> &output) const
{
    return Tasking::Sync([output] { *output = QList<Utils::Port>{}; });
}

ProjectExplorer::IDeviceWidget *FreeboxDevice::createWidget()
{
    return 0;
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

void FreeboxDevice::fromMap(const Utils::Store &map)
{
    IDevice::fromMap(map);
    mAddress = QHostAddress(map.value("address").toString());
}

void FreeboxDevice::toMap(Utils::Store &map) const
{
    IDevice::toMap(map);
    map.insert("address", mAddress.toString());
}

class FreeboxDeviceFactory final : public ProjectExplorer::IDeviceFactory
{
public:
    FreeboxDeviceFactory()
        : IDeviceFactory(Constants::FREEBOX_DEVICE_TYPE)
    {
        setDisplayName("Freebox Device");
        setCombinedIcon(":/freebox/images/tvdevicesmall.png", ":/freebox/images/tvdevice.png");
        setConstructionFunction([] { return IDevice::Ptr(new FreeboxDevice); });
    }
};

void setupFreeboxDevice()
{
    static FreeboxDeviceFactory theFreeboxDeviceFactory;
}

} // namespace Freebox::Internal
