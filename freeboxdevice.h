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

namespace Freebox {

namespace Internal {
    class FreeboxConfiguration;
} // namespace Internal

class FreeboxDevice final : public ProjectExplorer::IDevice
{
public:
    FreeboxDevice();

    typedef QSharedPointer<FreeboxDevice> Ptr;
    typedef QSharedPointer<const FreeboxDevice> ConstPtr;

    static IDevice::Ptr create();

    const QHostAddress &address() const;

protected:
    FreeboxDevice(Utils::Id id, const QHostAddress &addr);
    void fromMap(const QVariantMap &map) final;
    QVariantMap toMap() const final;

    friend class Internal::FreeboxConfiguration;

private:
    IDevice::DeviceInfo deviceInformation() const override;
    ProjectExplorer::IDeviceWidget *createWidget() override;
    bool canAutoDetectPorts() const override;
    ProjectExplorer::DeviceProcessSignalOperation::Ptr signalOperation() const override;
    QUrl toolControlChannel(const ControlChannelHint &) const override;

    QHostAddress mAddress;
};

class FreeboxDeviceFactory final : public ProjectExplorer::IDeviceFactory
{
public:
    FreeboxDeviceFactory();
};

} // namespace Freebox

