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

#include "freeboxconfiguration.h"
#include "freeboxconstants.h"
#include "freeboxdevice.h"
#include "ssdp/ssdpclient.h"
#include "ssdp/ssdpmessage.h"

#include <projectexplorer/devicesupport/idevice.h>
#include <projectexplorer/devicesupport/devicemanager.h>
#include <projectexplorer/kitmanager.h>
#include <projectexplorer/kit.h>
#include <projectexplorer/kitinformation.h>
#include <qtsupport/qtkitinformation.h>

#include <utils/qtcassert.h>

using namespace ProjectExplorer;

namespace Freebox {
namespace Internal {

FreeboxConfiguration::FreeboxConfiguration(QObject *parent) :
    QObject(parent),
    mSSDPClient(new Ssdp::Client(this))
{
    connect(mSSDPClient, &Ssdp::Client::messageReceived,
            this, &FreeboxConfiguration::filterMessage);
}

void FreeboxConfiguration::updateKits()
{
    qWarning() << "updateKits";

    QSet<QString> ids = mFreebox;

    for (Kit *kit : KitManager::kits()) {
        if (DeviceTypeKitAspect::deviceTypeId(kit) != Constants::FREEBOX_DEVICE_TYPE)
            continue;

        IDevice::ConstPtr device = DeviceKitAspect::device(kit);
        if (!device) {
            KitManager::deregisterKit(kit);
            continue;
        }

        if (ids.contains(device->id().toString()))
            ids.remove(device->id().toString());
        else
            KitManager::deregisterKit(kit);
    }

    for (const QString &usn : ids) {
        Utils::Id deviceId = Utils::Id::fromString(usn);

        IDevice::ConstPtr device = DeviceManager::instance()->find(deviceId);
        QTC_ASSERT(device, continue);

        FreeboxDevice::ConstPtr freebox =
                device.dynamicCast<const FreeboxDevice>();
        QTC_ASSERT(freebox, continue);

        const auto initializeKit = [deviceId, freebox](Kit *k) {
            k->setAutoDetected(true);
            k->setAutoDetectionSource("FreeboxConfiguration");
            k->setUnexpandedDisplayName(tr("Freebox (%1)")
                                            .arg(freebox->address().toString()));
            QtSupport::QtKitAspect::setQtVersion(k, 0);
            DeviceTypeKitAspect::setDeviceTypeId(k, Constants::FREEBOX_DEVICE_TYPE);
            DeviceKitAspect::setDeviceId(k, deviceId);
        };
        KitManager::registerKit(initializeKit);
    }
}

void FreeboxConfiguration::updateDevices()
{
    qWarning("UPDATE DEVICES");

    // Remove any dummy Android device, because it won't be usable.
    DeviceManager *const devMgr = DeviceManager::instance();
    IDevice::ConstPtr dev = devMgr->find(Constants::FREEBOX_DEVICE_ID);
    if (dev)
        devMgr->removeDevice(dev->id());

    mSSDPClient->start(QString::fromLocal8Bit(Constants::FREEBOX_DEVICE_NT));
    mSSDPClient->search();
}

void FreeboxConfiguration::freeboxAdd(const QHostAddress &addr,
                                      const QString &usn)
{
    if (mFreebox.contains(usn))
        return;

    qWarning() << "found a new freebox" << usn << addr.toString();

    FreeboxDevice *device =
            new FreeboxDevice(Utils::Id::fromString(usn), addr);
    device->setDeviceState(IDevice::DeviceReadyToUse);
    mFreebox.insert(usn);
    DeviceManager::instance()->addDevice(IDevice::Ptr(device));

    updateKits();
}

void FreeboxConfiguration::freeboxDel(const QString &usn)
{
    if (!mFreebox.contains(usn))
        return;

    qWarning() << "lost a freebox" << usn;

    mFreebox.remove(usn);
    DeviceManager::instance()->removeDevice(Utils::Id::fromString(usn));

    updateKits();
}

void FreeboxConfiguration::filterMessage(const QHostAddress &addr,
                                         const Ssdp::Message &message)
{
    switch (message.type()) {
    case Ssdp::Message::NOTIFY: {
        if (message.fieldValue(QStringLiteral("NT")) != QLatin1String(Constants::FREEBOX_DEVICE_NT))
            return;

        QString nts = message.fieldValue(QStringLiteral("NTS"));
        QString usn = message.fieldValue(QStringLiteral("USN"));

        if (nts == QLatin1String("ssdp:alive"))
            freeboxAdd(addr, usn);
        else if (nts == QLatin1String("ssdp:byebye"))
            freeboxDel(usn);
        break;
    }
    case Ssdp::Message::REPLY: {
        if (message.fieldValue(QStringLiteral("ST")) != QLatin1String(Constants::FREEBOX_DEVICE_NT))
            return;

        QString usn = message.fieldValue(QStringLiteral("USN"));
        freeboxAdd(addr, usn);
        break;
    }
    default:
        break;
    }
}

} // namespace Internal
} // namespace Freebox
