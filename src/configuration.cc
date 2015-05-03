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

#include "configuration.hh"
#include "constants.hh"
#include "device.hh"
#include "ssdp/ssdpclient.hh"
#include "ssdp/ssdpmessage.hh"

#include <projectexplorer/devicesupport/idevice.h>
#include <projectexplorer/devicesupport/devicemanager.h>
#include <projectexplorer/kitmanager.h>
#include <projectexplorer/kit.h>
#include <projectexplorer/kitinformation.h>
#include <qtsupport/qtkitinformation.h>

#include <utils/qtcassert.h>

namespace Freebox {
namespace Internal {

FreeboxConfiguration::FreeboxConfiguration(QObject *parent) :
    QObject(parent),
    mSSDPClient(new Ssdp::Client(this))
{
    connect(mSSDPClient, SIGNAL(messageReceived(const QHostAddress &,
                                                const Ssdp::Message &)),
            SLOT(filterMessage(const QHostAddress &,
                               const Ssdp::Message &)));
    connect(this, SIGNAL(freeboxAlive(const QHostAddress &,
                                      const QString &)),
            SLOT(freeboxAdd(const QHostAddress &,
                            const QString &)));
    connect(this, SIGNAL(freeboxByebye(const QString &)),
            SLOT(freeboxDel(const QString &)));
}

void FreeboxConfiguration::updateKits()
{
    using ProjectExplorer::KitManager;
    using ProjectExplorer::Kit;
    using ProjectExplorer::DeviceTypeKitInformation;
    using ProjectExplorer::DeviceKitInformation;
    using ProjectExplorer::DeviceManager;
    QStringList ids = mFreebox.keys();

    foreach (Kit *kit, KitManager::kits()) {
        if (DeviceTypeKitInformation::deviceTypeId(kit) !=
                Core::Id(Constants::FREEBOX_DEVICE_TYPE))
            continue;

        ProjectExplorer::IDevice::ConstPtr device =
                DeviceKitInformation::device(kit);

        if (!device) {
            KitManager::deregisterKit(kit);
            continue;
        }

        if (ids.contains(device->id().toString()))
            ids.removeAll(device->id().toString());
        else
            KitManager::deregisterKit(kit);
    }

    foreach(QString usn, ids) {
        Core::Id deviceId = Core::Id(mFreebox[usn]->toLocal8Bit().constData());

        ProjectExplorer::IDevice::ConstPtr device =
                DeviceManager::instance()->find(deviceId);
        QTC_ASSERT(device, continue);

        FreeboxDevice::ConstPtr freebox =
                device.dynamicCast<const FreeboxDevice>();
        QTC_ASSERT(freebox, continue);

        Kit *kit = new Kit();
        kit->setUnexpandedDisplayName(tr("Freebox (%1)").arg(freebox->address().toString()));
        kit->setAutoDetected(true);
        QtSupport::QtKitInformation::setQtVersion(kit, 0);
        DeviceTypeKitInformation::setDeviceTypeId(kit, Core::Id(Constants::FREEBOX_DEVICE_TYPE));
        DeviceKitInformation::setDeviceId(kit, deviceId);
        kit->makeSticky();
        KitManager::registerKit(kit);
    }
}

void FreeboxConfiguration::updateDevices()
{
    mSSDPClient->start(QString::fromLocal8Bit(Constants::FREEBOX_DEVICE_NT));
    mSSDPClient->search();
}

void FreeboxConfiguration::freeboxAdd(const QHostAddress &addr,
                                      const QString &usn)
{
    using ProjectExplorer::DeviceManager;
    using ProjectExplorer::IDevice;

    if (mFreebox.contains(usn))
        return;

    QString *id = new QString(usn);
    FreeboxDevice *device =
            new FreeboxDevice(Core::Id(id->toLocal8Bit().constData()), addr);
    device->setDeviceState(ProjectExplorer::IDevice::DeviceReadyToUse);
    mFreebox.insert(usn, id);
    DeviceManager::instance()->addDevice(IDevice::Ptr(device));
    qWarning() << "found a new freebox" << *id << addr.toString();

    updateKits();
}

void FreeboxConfiguration::freeboxDel(const QString &usn)
{
    using ProjectExplorer::DeviceManager;

    if (!mFreebox.contains(usn))
        return;

    QString *id = mFreebox.take(usn);
    qWarning() << "lost a freebox" << *id;
    DeviceManager::instance()->removeDevice(Core::Id(id->toLocal8Bit().constData()));
    delete id;

    updateKits();
}

void FreeboxConfiguration::filterMessage(const QHostAddress &addr,
                                         const Ssdp::Message &message)
{
    switch (message.type()) {
    case Ssdp::Message::NOTIFY: {
        if (message.fieldValue(QStringLiteral("NT")) != QString::fromLocal8Bit(Constants::FREEBOX_DEVICE_NT))
            return;

        QString nts = message.fieldValue(QStringLiteral("NTS"));
        QString usn = message.fieldValue(QStringLiteral("USN"));

        if (nts == QStringLiteral("ssdp:alive"))
            freeboxAlive(addr, usn);
        else if (nts == QStringLiteral("ssdp:byebye"))
            freeboxByebye(usn);
        break;
    }
    case Ssdp::Message::REPLY: {
        if (message.fieldValue(QStringLiteral("ST")) != QString::fromLocal8Bit(Constants::FREEBOX_DEVICE_NT))
            return;

        QString usn = message.fieldValue(QStringLiteral("USN"));
        freeboxAlive(addr, usn);
        break;
    }
    default:
        break;
    }
}

} // namespace Internal
} // namespace Freebox
