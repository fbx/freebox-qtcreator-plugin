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
#pragma once

#include <projectexplorer/devicesupport/idevice.h>
#include <projectexplorer/devicesupport/idevicefactory.h>

#include <QNetworkInterface>
#include <QHostAddress>

namespace Freebox::Internal {

class FreeboxConfiguration;

class FreeboxDevice final : public ProjectExplorer::IDevice
{
public:
    FreeboxDevice();

    using ConstPtr = std::shared_ptr<const FreeboxDevice>;
    using Ptr = std::shared_ptr<FreeboxDevice>;
    const QHostAddress &address() const;

protected:
    void configure(const QString &usn, const QHostAddress &addr);
    void fromMap(const Utils::Store  &map) final;
    void toMap(Utils::Store &map) const final;

    friend class FreeboxConfiguration;

private:
    IDevice::DeviceInfo deviceInformation() const override;
    ProjectExplorer::IDeviceWidget *createWidget() override;
    ProjectExplorer::DeviceProcessSignalOperation::Ptr signalOperation() const override;
    QUrl toolControlChannel(const ControlChannelHint &) const override;
    Utils::Result<Utils::Environment> systemEnvironmentWithError() const override;
    Tasking::ExecutableItem portsGatheringRecipe(
        const Tasking::Storage<Utils::PortsOutputData> &output) const override;

    QHostAddress mAddress;
};

void setupFreeboxDevice();
} // namespace Freebox

