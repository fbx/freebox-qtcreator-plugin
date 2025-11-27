#include "freeboxrunconfiguration.h"
#include "freeboxconstants.h"

#include <projectexplorer/runconfiguration.h>
#include <qmlprojectmanager/qmlprojectconstants.h>

using namespace ProjectExplorer;
using namespace Utils;

namespace Freebox::Internal {

class FreeboxRunConfiguration : public RunConfiguration
{
public:
    FreeboxRunConfiguration(BuildConfiguration *bc, Id id)
        : RunConfiguration(bc, id)
    {
        setUsesEmptyBuildKeys();
    }
};

class FreeboxRunConfigurationFactory : public FixedRunConfigurationFactory
{
public:
    FreeboxRunConfigurationFactory()
        : FixedRunConfigurationFactory(Freebox::Constants::FREEBOX_RUNTIME, true)
    {
        registerRunConfiguration<FreeboxRunConfiguration>(Freebox::Constants::FREEBOX_RUNCONFIG_ID);
        addSupportedTargetDeviceType(Freebox::Constants::FREEBOX_DEVICE_TYPE);
        addSupportedProjectType(QmlProjectManager::Constants::QML_PROJECT_ID);
    }
};

void setupFreeboxRunConfiguration()
{
    static FreeboxRunConfigurationFactory theFreeboxRunConfigurationFactory;
}

} // namespace Freebox::Internal
