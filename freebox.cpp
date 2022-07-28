#include "freebox.h"
#include "freeboxconfiguration.h"
#include "freeboxconstants.h"
#include "freeboxdevice.h"
#include "freeboxqmltoolingsupport.h"
#include "freeboxrunconfiguration.h"
#include "freeboxruncontrol.h"

#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/devicesupport/devicemanager.h>
#include <projectexplorer/kitmanager.h>
#include <projectexplorer/runcontrol.h>

#include <qmlprojectmanager/qmlprojectmanagerconstants.h>

using namespace ProjectExplorer;
using namespace ProjectExplorer::Constants;

namespace Freebox {
namespace Internal {

class FreeboxRunConfigurationFactory : public FixedRunConfigurationFactory
{
public:
    FreeboxRunConfigurationFactory()
        : FixedRunConfigurationFactory("Freebox Runtime", true)
    {
        registerRunConfiguration<FreeboxRunConfiguration>("FreeboxRunConfiguration");
        addSupportedTargetDeviceType(Constants::FREEBOX_DEVICE_TYPE);
        addSupportedProjectType(QmlProjectManager::Constants::QML_PROJECT_ID);
    }
};

FreeboxPlugin::FreeboxPlugin()
{
}

FreeboxPlugin::~FreeboxPlugin()
{
}

bool FreeboxPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    FreeboxConfiguration *configuration = new FreeboxConfiguration(this);

    new FreeboxDeviceFactory;
    auto runConfigFactory = new FreeboxRunConfigurationFactory();

    new RunWorkerFactory(
        RunWorkerFactory::make<Freebox::RunControl>(),
        { NORMAL_RUN_MODE },
        { runConfigFactory->runConfigurationId() },
        { Freebox::Constants::FREEBOX_DEVICE_TYPE }
    );

    new RunWorkerFactory(
        RunWorkerFactory::make<FreeboxQmlToolingSupport>(),
        { QML_PROFILER_RUN_MODE, QML_PREVIEW_RUN_MODE },
        { runConfigFactory->runConfigurationId() },
        { Freebox::Constants::FREEBOX_DEVICE_TYPE }
    );

    connect(KitManager::instance(), &KitManager::kitsLoaded,
            configuration, &FreeboxConfiguration::updateKits);
    connect(DeviceManager::instance(), &DeviceManager::devicesLoaded,
            configuration, &FreeboxConfiguration::updateDevices);

    return true;
}

void FreeboxPlugin::extensionsInitialized()
{
}

ExtensionSystem::IPlugin::ShutdownFlag FreeboxPlugin::aboutToShutdown()
{
    return SynchronousShutdown;
}

} // namespace Internal
} // namespace Freebox
