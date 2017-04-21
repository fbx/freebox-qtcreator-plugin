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

#include "freeboxplugin.hh"
#include "constants.hh"
#include "configuration.hh"
#include "runconfigurationfactory.hh"
#include "runcontrolfactory.hh"
#include "manager.hh"
#include "project.hh"
#include "devicefactory.hh"
#include "device.hh"
#include "freestorepackager.hh"

#include <coreplugin/icore.h>
#include <coreplugin/icontext.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/fileiconprovider.h>
#include <projectexplorer/devicesupport/devicemanager.h>
#include <projectexplorer/jsonwizard/jsonwizardfactory.h>
#include <projectexplorer/kitmanager.h>
#include <projectexplorer/session.h>

#include <utils/mimetypes/mimedatabase.h>

#include <QAction>
#include <QMessageBox>
#include <QMainWindow>
#include <QMenu>
#include <QResource>
#include <QDebug>
#include <QFileDialog>

namespace Freebox {
namespace Internal {

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
    //
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

    Utils::MimeDatabase::addMimeTypes(QLatin1String(":/freebox/FbxProjectManager.mimetypes.xml"));
    ProjectExplorer::JsonWizardFactory::addWizardPath(Utils::FileName::fromString(QStringLiteral(":/freebox/templates/wizard")));

    FreeboxConfiguration *configuration = new FreeboxConfiguration(this);

    addAutoReleasedObject(new DeviceFactory);
    addAutoReleasedObject(new RunControlFactory);
    addAutoReleasedObject(new Internal::Manager);
    addAutoReleasedObject(new RunConfigurationFactory);

    connect(ProjectExplorer::KitManager::instance(), SIGNAL(kitsLoaded()),
            configuration, SLOT(updateKits()));
    connect(ProjectExplorer::DeviceManager::instance(), SIGNAL(devicesLoaded()),
            configuration, SLOT(updateDevices()));

    Core::FileIconProvider::registerIconOverlayForSuffix(":/freebox/images/qmlproject.png",
                                                         "fbxproject");

    m_actionMakePackage = new QAction(tr("Make FreeStore &Package"), this);
    m_actionMakePackage->setEnabled(false);
    Core::Command *cmd = Core::ActionManager::registerAction(m_actionMakePackage, Constants::MAKE_PACKAGE_ACTION_ID,
                                                             Core::Context(Core::Constants::C_GLOBAL));
    cmd->setDefaultKeySequence(QKeySequence(tr("Ctrl+Alt+P")));
    connect(m_actionMakePackage, SIGNAL(triggered()), this, SLOT(makeFreeStorePackageAction()));

    m_actionMakePackageAs = new QAction(tr("Make FreeStore &Package As..."), this);
    m_actionMakePackageAs->setEnabled(false);
    Core::Command *cmdAs = Core::ActionManager::registerAction(m_actionMakePackageAs, Constants::MAKE_PACKAGE_AS_ACTION_ID,
                                                               Core::Context(Core::Constants::C_GLOBAL));
    cmdAs->setDefaultKeySequence(QKeySequence(tr("Ctrl+Alt+Shift+P")));
    connect(m_actionMakePackageAs, SIGNAL(triggered()), this, SLOT(makeFreeStorePackageAsAction()));

    Core::ActionContainer *menu = Core::ActionManager::createMenu(Constants::MENU_ID);
    menu->menu()->setTitle(tr("&Freebox SDK"));
    menu->addAction(cmd);
    menu->addAction(cmdAs);
    Core::ActionManager::actionContainer(Core::Constants::M_TOOLS)->addMenu(menu);

    connect(ProjectExplorer::SessionManager::instance(),
            SIGNAL(startupProjectChanged(ProjectExplorer::Project*)),
            SLOT(onCurrentProjectChanged(ProjectExplorer::Project*)));

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
    disconnect(ProjectExplorer::SessionManager::instance(), 0, this, 0);
    // Hide UI (if you add UI that is not in the main window directly)
    return SynchronousShutdown;
}

void FreeboxPlugin::onCurrentProjectChanged(ProjectExplorer::Project *project)
{
    bool enabled = project ? (typeid(*project) == typeid(Freebox::Project)) : false;

    m_actionMakePackage->setEnabled(enabled);
    m_actionMakePackageAs->setEnabled(enabled);
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
    ProjectExplorer::Project *project = ProjectExplorer::SessionManager::startupProject();

    QTC_ASSERT(project && (typeid(*project) == typeid(Freebox::Project)), return); // just in case

    Freebox::Project *fbxProject = qobject_cast<Freebox::Project *>(project);

    QString packageFileName = fbxProject->packageFileName();

    if (packageFileName.isEmpty())
        fbxProject->setPackageFileName(packageFileName = fbxProject->projectDir().canonicalPath() + QString::fromLocal8Bit(".fbxqml"));

    if (saveAs) {
        QString outFileName =
            QFileDialog::getSaveFileName(Core::ICore::mainWindow(),
                                         tr("Make FreeStore Package"),
                                         packageFileName,
                                         tr("Freebox QML package (*.fbxqml)"));
        if (outFileName.isEmpty())
            return;

        fbxProject->setPackageFileName(packageFileName = outFileName);
    }

    Freebox::Internal::FreeStorePackager packager(fbxProject, packageFileName);
    if (packager()) {
        QMessageBox::information(Core::ICore::mainWindow(),
                                 tr("Make FreeStore Package Successful"),
                                 tr("FreeStore package for project %1 done in '%2'.")
                                 .arg(fbxProject->displayName())
                                 .arg(packager.outFileName()));
    }
}

} // namespace Internal
} // namespace Freebox
