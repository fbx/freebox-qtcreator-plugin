#include "freeboxconfiguration.h"
#include "freeboxconstants.h"
#include "freeboxdevice.h"
#include "freeboxlog.h"
#include "freeboxrunconfiguration.h"
#include "freeboxrunner.h"
#include "freestorepackager.h"

#include <extensionsystem/iplugin.h>

#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <projectexplorer/buildmanager.h>
#include <projectexplorer/buildsystem.h>
#include <projectexplorer/devicesupport/devicemanager.h>
#include <projectexplorer/jsonwizard/jsonwizardfactory.h>
#include <projectexplorer/kitmanager.h>
#include <projectexplorer/project.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/projectmanager.h>
#include <projectexplorer/runconfiguration.h>
#include <projectexplorer/target.h>
#include <projectexplorer/taskhub.h>

#include <coreplugin/icontext.h>
#include <coreplugin/icore.h>

#include <utils/fsengine/fileiconprovider.h>
#include <utils/mimeconstants.h>

#include <qmlprojectmanager/qmlproject.h>
#include <qmlprojectmanager/qmlprojectconstants.h>

#include <QAction>
#include <QFileDialog>
#include <QMainWindow>
#include <QMenu>
#include <QMessageBox>

using namespace ProjectExplorer;
using namespace Core;
using namespace Utils;

namespace Freebox {
Q_LOGGING_CATEGORY(fbxLog, "freebox")
}

namespace Freebox::Internal {

class FreeboxPlugin final : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "Freebox.json")

public:
    void initialize(void) final;

private slots:
    void onProjectAdded(Project *project);

private:
    void createMenus();
    void makeFreeStorePackageAction();
    void makeFreeStorePackageAsAction();
    void makeFreeStorePackage(bool saveAs);

    QAction *m_saveAction = nullptr;
    QAction *m_saveAsAction = nullptr;
};

void FreeboxPlugin::initialize(void)
{
    ProjectManager::registerProjectType<QmlProjectManager::QmlProject>(
        Utils::Constants::QMLPROJECT_MIMETYPE);
    ProjectManager::registerProjectType<QmlProjectManager::QmlProject>(
        Constants::FBXPROJECT_MIMETYPE);

    setupFreeboxConfiguration();

    setupFreeboxDevice();
    setupFreeboxRunConfiguration();

    setupFreeboxRunWorker();

    createMenus();

    connect(
        KitManager::instance(),
        &KitManager::kitsLoaded,
        FreeboxConfiguration::instance(),
        &FreeboxConfiguration::updateKits);

    connect(
        DeviceManager::instance(),
        &DeviceManager::devicesLoaded,
        FreeboxConfiguration::instance(),
        &FreeboxConfiguration::updateDevices);

    //Enable the menu only if a QML project is loaded
    connect(
        ProjectManager::instance(),
        &ProjectManager::projectAdded,
        this,
        &FreeboxPlugin::onProjectAdded);
}

void FreeboxPlugin::createMenus()
{
    MenuBuilder FreeboxMenu(Constants::FREEBOX_SDK_MENU_ID);
    FreeboxMenu.setTitle("&Freebox SDK");
    FreeboxMenu.addToContainer(Core::Constants::M_TOOLS);

    ActionBuilder toggleSaveAction(this, Constants::MAKE_PKG_ACTION_ID);
    toggleSaveAction.setText("Make FreeStore &Package");
    toggleSaveAction.setDefaultKeySequence(tr("Ctrl+Alt+P"));
    toggleSaveAction.addToContainer(Constants::FREEBOX_SDK_MENU_ID);
    toggleSaveAction.bindContextAction(&m_saveAction);
    toggleSaveAction.addOnTriggered(this, &FreeboxPlugin::makeFreeStorePackageAction);

    ActionBuilder toggleSaveAsAction(this, Constants::MAKE_PKG_AS_ACTION_ID);
    toggleSaveAsAction.setText("Make FreeStore &Package As...");
    toggleSaveAsAction.setDefaultKeySequence(tr("Ctrl+Alt+Shift+P"));
    toggleSaveAsAction.addToContainer(Constants::FREEBOX_SDK_MENU_ID);
    toggleSaveAsAction.bindContextAction(&m_saveAsAction);
    toggleSaveAsAction.addOnTriggered(this, &FreeboxPlugin::makeFreeStorePackageAsAction);

    m_saveAction->setEnabled(false);
    m_saveAsAction->setEnabled(false);
}

void FreeboxPlugin::onProjectAdded(Project *project)
{
    Target *target = project->activeTarget();
    if (!target)
        return;

    BuildSystem *bs = target->buildSystem();
    if (!bs)
        return;

    bool enabled = project ? (project->type() == QmlProjectManager::Constants::QML_PROJECT_ID)
                           : false;

    m_saveAction->setEnabled(enabled);
    m_saveAsAction->setEnabled(enabled);
}

void FreeboxPlugin::makeFreeStorePackageAction()
{
    makeFreeStorePackage(false);
}

void FreeboxPlugin::makeFreeStorePackageAsAction()
{
    makeFreeStorePackage(true);
}

void FreeboxPlugin::makeFreeStorePackage(bool saveAs)
{
    Project *currentProject = ProjectManager::startupProject();
    QTC_ASSERT(
        currentProject && (currentProject->type() == QmlProjectManager::Constants::QML_PROJECT_ID),
        return);

    Target *target = currentProject->activeTarget();
    if (!target) {
        qCWarning(fbxLog) << "No active target";
        return;
    }

    BuildSystem *bs = target->buildSystem();
    if (!bs) {
        qCWarning(fbxLog) << "No build system found for the active target.";
        return;
    }

    QString packageFileName = currentProject->projectDirectory().toFSPathString()
                              + QString::fromLocal8Bit(".fbxqml");

    if (saveAs) {
        QString outFileName = QFileDialog::getSaveFileName(
            Core::ICore::mainWindow(),
            tr("Make FreeStore Package"),
            packageFileName,
            tr("Freebox QML package (*.fbxqml)"));
        if (outFileName.isEmpty())
            return;
        packageFileName = outFileName;
    }

    Freebox::Internal::FreeStorePackager packager(currentProject, packageFileName);
    if (packager()) {
        QMessageBox::information(
            Core::ICore::mainWindow(),
            tr("Make FreeStore Package Successful"),
            tr("FreeStore package for project %1 done in '%2'.")
                .arg(currentProject->displayName())
                .arg(packager.outFileName()));
    }
}

} // namespace Freebox::Internal

#include "freebox.moc"
