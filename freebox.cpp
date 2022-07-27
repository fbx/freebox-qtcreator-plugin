#include "freebox.h"
#include "freeboxconfiguration.h"
#include "freeboxconstants.h"
#include "freeboxdevice.h"
#include "freeboxruncontrol.h"

#include <coreplugin/icore.h>
#include <coreplugin/icontext.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>

#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/devicesupport/devicemanager.h>
#include <projectexplorer/kitmanager.h>
#include <projectexplorer/runcontrol.h>

#include <QAction>
#include <QMessageBox>
#include <QMainWindow>
#include <QMenu>

using namespace ProjectExplorer;
using namespace ProjectExplorer::Constants;

namespace Freebox {
namespace Internal {

class FreeboxRunConfigurationFactory : public RunConfigurationFactory
{
public:
    FreeboxRunConfigurationFactory()
    {
        addSupportedTargetDeviceType(Constants::FREEBOX_DEVICE_TYPE);
    }
};

FreeboxPlugin::FreeboxPlugin()
{
    // Create your members
}

FreeboxPlugin::~FreeboxPlugin()
{
    // Unregister objects from the plugin manager's object pool
    // Delete members
}

bool FreeboxPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    // Register objects in the plugin manager's object pool
    // Load settings
    // Add actions to menus
    // Connect to other plugins' signals
    // In the initialize function, a plugin can be sure that the plugins it
    // depends on have initialized their members.

    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    FreeboxConfiguration *configuration = new FreeboxConfiguration(this);

    new FreeboxDeviceFactory;
    auto runConfigFactory = new FreeboxRunConfigurationFactory();

    new RunWorkerFactory(
        RunWorkerFactory::make<Freebox::RunControl>(),
        { NORMAL_RUN_MODE },
        { runConfigFactory->runConfigurationId() }
    );

    connect(KitManager::instance(), &KitManager::kitsLoaded,
            configuration, &FreeboxConfiguration::updateKits);
    connect(DeviceManager::instance(), &DeviceManager::devicesLoaded,
            configuration, &FreeboxConfiguration::updateDevices);

    auto action = new QAction(tr("Freebox Action"), this);
    Core::Command *cmd = Core::ActionManager::registerAction(action, Constants::ACTION_ID,
                                                             Core::Context(Core::Constants::C_GLOBAL));
    cmd->setDefaultKeySequence(QKeySequence(tr("Ctrl+Alt+Meta+A")));
    connect(action, &QAction::triggered, this, &FreeboxPlugin::triggerAction);

    Core::ActionContainer *menu = Core::ActionManager::createMenu(Constants::MENU_ID);
    menu->menu()->setTitle(tr("Freebox"));
    menu->addAction(cmd);
    Core::ActionManager::actionContainer(Core::Constants::M_TOOLS)->addMenu(menu);

    return true;
}

void FreeboxPlugin::extensionsInitialized()
{
    // Retrieve objects from the plugin manager's object pool
    // In the extensionsInitialized function, a plugin can be sure that all
    // plugins that depend on it are completely initialized.
}

ExtensionSystem::IPlugin::ShutdownFlag FreeboxPlugin::aboutToShutdown()
{
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    return SynchronousShutdown;
}

void FreeboxPlugin::triggerAction()
{
    QMessageBox::information(Core::ICore::mainWindow(),
                             tr("Action Triggered"),
                             tr("This is an action from Freebox."));
}

} // namespace Internal
} // namespace Freebox
