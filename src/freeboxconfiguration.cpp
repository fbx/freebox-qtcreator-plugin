#include "freeboxconfiguration.h"
#include "freeboxconstants.h"
#include "freeboxdevice.h"
#include "freeboxlog.h"

#include "ssdp/ssdpclient.h"
#include "ssdp/ssdpmessage.h"

#include <debugger/debuggerkitaspect.h>

#include <projectexplorer/devicesupport/devicekitaspects.h>
#include <projectexplorer/devicesupport/devicemanager.h>
#include <projectexplorer/environmentkitaspect.h>
#include <projectexplorer/kitmanager.h>
#include <projectexplorer/project.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/projectmanager.h>
#include <projectexplorer/sysrootkitaspect.h>
#include <projectexplorer/toolchainkitaspect.h>
#include <projectexplorer/toolchainmanager.h>

#include <qtsupport/qtkitaspect.h>
#include <qtsupport/qtsupportconstants.h>
#include <qtsupport/qtversionmanager.h>

#include <utils/qtcassert.h>

using namespace ProjectExplorer;

namespace Freebox::Internal {

FreeboxConfiguration *m_instance = nullptr;

FreeboxConfiguration::FreeboxConfiguration()
    : mSSDPClient(new Ssdp::Client(this))
{
    m_instance = this;
    connect(mSSDPClient, &Ssdp::Client::messageReceived, this, &FreeboxConfiguration::filterMessage);
}

FreeboxConfiguration *FreeboxConfiguration::instance()
{
    return m_instance;
}

void FreeboxConfiguration::updateKits()
{
    Kit *existingKit = nullptr;

    for (Kit *kit : KitManager::kits()) {
        if (RunDeviceTypeKitAspect::deviceTypeId(kit) == Constants::FREEBOX_DEVICE_TYPE) {
            if (kit->id() == Constants::FREEBOX_KIT_ID)
                existingKit = kit;
            else
                KitManager::deregisterKit(kit);
        }
    }

    const auto initializeKit = [](Kit *k) {
        k->setDetectionSource({DetectionSource::FromSystem, "FreeboxConfiguration"});
        k->setUnexpandedDisplayName("Freebox");
        k->setSticky(RunDeviceTypeKitAspect::id(), true);
        RunDeviceTypeKitAspect::setDeviceTypeId(k, Constants::FREEBOX_DEVICE_TYPE);
    };

    if (!existingKit)
        KitManager::registerKit(initializeKit, Constants::FREEBOX_KIT_ID);
    else
        initializeKit(existingKit);
}

void FreeboxConfiguration::updateDevices()
{
    // Remove any dummy Freebox device, because it won't be usable.
    DeviceManager *const devMgr = DeviceManager::instance();
    IDevice::ConstPtr dev = devMgr->find(Constants::FREEBOX_DEVICE_ID);
    if (dev)
        devMgr->removeDevice(dev->id());

    mSSDPClient->start(QString::fromLocal8Bit(Constants::FREEBOX_DEVICE_NT));
    mSSDPClient->search();
}

void FreeboxConfiguration::freeboxAdd(const QHostAddress &addr, const QString &usn)
{
    if (mFreebox.contains(usn))
        return;

    qCDebug(fbxLog) << "found a new freebox" << usn << addr.toString();

    IDeviceFactory *factory = IDeviceFactory::find(Constants::FREEBOX_DEVICE_TYPE);
    QTC_ASSERT(factory, return);
    IDevice::Ptr device = factory->construct();
    QTC_ASSERT(device, return);

    std::static_pointer_cast<FreeboxDevice>(device)->configure(usn, addr);
    device->setDeviceState(IDevice::DeviceReadyToUse);
    mFreebox.insert(usn);
    DeviceManager::instance()->addDevice(device);
}

void FreeboxConfiguration::freeboxDel(const QString &usn)
{
    if (!mFreebox.contains(usn))
        return;

    qCDebug(fbxLog) << "lost a freebox" << usn;

    mFreebox.remove(usn);
    DeviceManager::instance()->removeDevice(Utils::Id::fromString(usn));
}

void FreeboxConfiguration::filterMessage(const QHostAddress &addr, const Ssdp::Message &message)
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

void setupFreeboxConfiguration()
{
    static FreeboxConfiguration theFreeboxConfiguration;
}

} // namespace Freebox::Internal
