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
#include <QAction>
#include <QMessageBox>
#include <QMainWindow>
#include <QMenu>
#include <QResource>
#include <QDebug>

#include <coreplugin/icore.h>
#include <coreplugin/icontext.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/mimedatabase.h>
#include <coreplugin/fileiconprovider.h>
#include <projectexplorer/devicesupport/devicemanager.h>
#include <projectexplorer/kitmanager.h>

#include "constants.hh"
#include "freeboxplugin.hh"
#include "configuration.hh"
#include "runconfigurationfactory.hh"
#include "runcontrolfactory.hh"
#include "manager.hh"
#include "wizard.hh"
#include "devicefactory.hh"
#include "device.hh"

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

    const QLatin1String mimetypesXml(":/freebox/FbxProjectManager.mimetypes.xml");

    if (!Core::MimeDatabase::addMimeTypes(mimetypesXml, errorString))
        return false;

    FreeboxConfiguration *configuration = new FreeboxConfiguration(this);

    addAutoReleasedObject(new DeviceFactory);
    addAutoReleasedObject(new RunControlFactory);
    addAutoReleasedObject(new Internal::Manager);
    addAutoReleasedObject(new RunConfigurationFactory);
    addAutoReleasedObject(new Internal::Wizard);

    connect(ProjectExplorer::KitManager::instance(), SIGNAL(kitsLoaded()),
            configuration, SLOT(updateKits()));
    connect(ProjectExplorer::DeviceManager::instance(), SIGNAL(devicesLoaded()),
            configuration, SLOT(updateDevices()));

    Core::FileIconProvider::registerIconOverlayForSuffix(":/freebox/images/qmlproject.png",
                                                         "fbxproject");
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

} // namespace Internal
} // namespace Freebox

Q_EXPORT_PLUGIN2(Freebox, FreeboxPlugin)

