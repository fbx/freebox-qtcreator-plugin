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
#ifndef DEVICE_HH_
# define DEVICE_HH_

#include <projectexplorer/devicesupport/idevice.h>

#include <QNetworkInterface>
#include <QHostAddress>

namespace Freebox {

namespace Internal {
    class FreeboxConfiguration;
    class DeviceFactory;
} // namespace Internal

class FreeboxDevice : public ProjectExplorer::IDevice
{
public:
    typedef QSharedPointer<FreeboxDevice> Ptr;
    typedef QSharedPointer<const FreeboxDevice> ConstPtr;

    ~FreeboxDevice() override;
    IDevice::DeviceInfo deviceInformation() const override;

    QString displayType() const override;
    ProjectExplorer::IDeviceWidget *createWidget() override;
    QList<Core::Id> actionIds() const override;
    QString displayNameForActionId(Core::Id actionId) const override;
    void executeAction(Core::Id actionId, QWidget *parent = 0) override;

    bool canAutoDetectPorts() const override;
    bool canCreateProcessModel() const override;
    ProjectExplorer::DeviceProcessSignalOperation::Ptr signalOperation() const override;

    IDevice::Ptr clone() const override;

    ProjectExplorer::Connection toolControlChannel(const ControlChannelHint &) const override;

    const QHostAddress &address() const;

protected:
    FreeboxDevice(Core::Id id, const QHostAddress &addr);
    FreeboxDevice(const FreeboxDevice &other);

    friend class Internal::FreeboxConfiguration;
    friend class Internal::DeviceFactory;

private:
    QHostAddress mAddress;
};

} // namespace Freebox

#endif /* !DEVICE_HH_ */
