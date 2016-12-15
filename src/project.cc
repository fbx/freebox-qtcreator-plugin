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

#include "project.hh"
#include "file.hh"
#include "fileformat/qmlprojectfileformat.hh"
#include "fileformat/qmlprojectitem.hh"
#include "fileformat/manifest.hh"
#include "remoterunconfiguration.hh"
#include "localrunconfiguration.hh"
#include "constants.hh"
#include "node.hh"
#include "manager.hh"

#include <coreplugin/icontext.h>
#include <coreplugin/icore.h>
#include <coreplugin/messagemanager.h>
#include <coreplugin/documentmanager.h>
#include <qtsupport/baseqtversion.h>
#include <qtsupport/qtkitinformation.h>
#include <qmljs/qmljsmodelmanagerinterface.h>
#include <projectexplorer/kitinformation.h>
#include <projectexplorer/kitmanager.h>
#include <projectexplorer/target.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/buildtargetinfo.h>
#include <qtsupport/qtsupportconstants.h>

namespace Freebox {

Project::Project(Internal::Manager *manager, const Utils::FileName &fileName)
{
    setId("FbxProjectManager.FbxProject");
    setProjectManager(manager);
    setDocument(new Internal::File(this, fileName));
    Core::DocumentManager::addDocument(document(), true);
    setRootProjectNode(new Internal::Node(this));

    setProjectContext(Core::Context(Constants::PROJECTCONTEXT));
    setProjectLanguages(Core::Context(ProjectExplorer::Constants::LANG_QMLJS));

    m_projectName = projectFilePath().toFileInfo().completeBaseName();

    projectManager()->registerProject(this);

    connect(ProjectExplorer::KitManager::instance(), &ProjectExplorer::KitManager::kitsChanged,
            this, &Project::updateKit);
}

Project::~Project()
{
    projectManager()->unregisterProject(this);

    delete m_projectItem.data();
}

void Project::addedTarget(ProjectExplorer::Target *target)
{
    connect(target, &ProjectExplorer::Target::addedRunConfiguration,
            this, &Project::addedRunConfiguration);
    foreach (ProjectExplorer::RunConfiguration *rc, target->runConfigurations())
        addedRunConfiguration(rc);
}

void Project::onActiveTargetChanged(ProjectExplorer::Target *target)
{
    if (m_activeTarget)
        disconnect(m_activeTarget, &ProjectExplorer::Target::kitChanged, this, &Project::onKitChanged);
    m_activeTarget = target;
    if (m_activeTarget)
        connect(target, &ProjectExplorer::Target::kitChanged, this, &Project::onKitChanged);

    // make sure e.g. the default qml imports are adapted
    refresh(RefreshConfiguration);
}

void Project::onKitChanged()
{
    // make sure e.g. the default qml imports are adapted
    refresh(RefreshConfiguration);
}

void Project::addedRunConfiguration(ProjectExplorer::RunConfiguration *rc)
{
    // The enabled state of qml runconfigurations can only be decided after
    // they have been added to a project
    RemoteRunConfiguration *remote = qobject_cast<RemoteRunConfiguration *>(rc);
    if (remote) {
        remote->updateEnabled();
        return;
    }

    LocalRunConfiguration *local = qobject_cast<LocalRunConfiguration *>(rc);
    if (local) {
        local->updateEnabled();
        return;
    }
}

QDir Project::projectDir() const
{
    return QDir(projectDirectory().toString());
}

Utils::FileName Project::filesFileName() const
{
    return projectFilePath();
}

void Project::parseProject(RefreshOptions options)
{
    if (options & RefreshFiles) {
        if (options & RefreshProjectFile) {
            delete m_projectItem.data();
        }

        m_manifest = Fileformat::Manifest(projectDir().canonicalPath()
                                          + QLatin1String("/manifest.json"));

        if (!m_projectItem) {
            QString errorMessage;
            m_projectItem =
                    QmlProjectManager::QmlProjectFileFormat::parseProjectFile(projectFilePath(),
                                                                              &errorMessage);
            if (m_projectItem) {
                connect(m_projectItem.data(), &QmlProjectManager::QmlProjectItem::qmlFilesChanged,
                        this, &Project::refreshFiles);

            } else {
                Core::MessageManager::write(tr("Error while loading project file %1.")
                                            .arg(projectFilePath().toUserOutput()),
                                            Core::MessageManager::NoModeSwitch);
                Core::MessageManager::write(errorMessage);
            }
        }

        if (m_projectItem) {
            m_projectItem->setSourceDirectory(projectDir().path());
            if (modelManager())
                modelManager()->updateSourceFiles(m_projectItem->files(), true);

            QString mainFilePath = m_projectItem->mainFile();
            if (!mainFilePath.isEmpty()) {
                mainFilePath = projectDir().absoluteFilePath(mainFilePath);
                Utils::FileReader reader;
                QString errorMessage;
                if (!reader.fetch(mainFilePath, &errorMessage)) {
                    Core::MessageManager::write(tr("Warning while loading project file %1.")
                                                .arg(projectFilePath().toUserOutput()));
                    Core::MessageManager::write(errorMessage);
                }
            }
        }
        rootProjectNode()->refresh();
    }

    if (options & RefreshConfiguration) {
        // update configuration
    }

    if (options & RefreshFiles)
        emit fileListChanged();
}

void Project::refresh(RefreshOptions options)
{
    parseProject(options);

    if (options & RefreshFiles)
        rootProjectNode()->refresh();

    if (!modelManager())
        return;

    QmlJS::ModelManagerInterface::ProjectInfo projectInfo =
            modelManager()->defaultProjectInfoForProject(this);
    foreach (const QString &searchPath, customImportPaths())
        projectInfo.importPaths.maybeInsert(Utils::FileName::fromString(searchPath),
                                            QmlJS::Dialect::Qml);

    modelManager()->updateProjectInfo(projectInfo, this);

    emit parsingFinished();
}

QmlJS::ModelManagerInterface *Project::modelManager() const
{
    return QmlJS::ModelManagerInterface::instance();
}

QStringList Project::files() const
{
    QStringList files;
    if (m_projectItem)
        files = m_projectItem->files();
    else
        files = m_files;
    return files;
}

QString Project::mainFile() const
{
    if (m_projectItem)
        return m_projectItem->mainFile();
    return QString();
}

bool Project::validProjectFile() const
{
    return !m_projectItem.isNull();
}

QStringList Project::customImportPaths() const
{
    QStringList importPaths;
    if (m_projectItem)
        importPaths = m_projectItem->importPaths();

    return importPaths;
}

bool Project::addFiles(const QStringList &filePaths)
{
    QStringList toAdd;
    foreach (const QString &filePath, filePaths) {
        if (!m_projectItem->matchesFile(filePath))
            toAdd << filePaths;
    }
    return toAdd.isEmpty();
}

void Project::refreshProjectFile()
{
    refresh(RefreshProjectFile | RefreshFiles);
}

void Project::refreshFiles(const QSet<QString> &/*added*/, const QSet<QString> &removed)
{
    refresh(RefreshFiles);
    if (!removed.isEmpty() && modelManager())
        modelManager()->removeFiles(removed.toList());
}

QString Project::displayName() const
{
    return m_projectName;
}

Internal::Manager *Project::projectManager() const
{
    return static_cast<Internal::Manager *>(ProjectExplorer::Project::projectManager());
}

bool Project::supportsKit(ProjectExplorer::Kit *k, QString *errorMessage) const
{
    using ProjectExplorer::DeviceTypeKitInformation;

    Core::Id deviceTypeId = DeviceTypeKitInformation::deviceTypeId(k);

    if (deviceTypeId == Constants::FREEBOX_DEVICE_TYPE) {
        return true;

    } else if (deviceTypeId == ProjectExplorer::Constants::DESKTOP_DEVICE_TYPE) {
        QtSupport::BaseQtVersion *version = QtSupport::QtKitInformation::qtVersion(k);

        if (!version) {
            if (errorMessage)
                *errorMessage = tr("No Qt version set in kit.");
            return false;
        }

        if (version->qtVersion() < QtSupport::QtVersionNumber(5, 0, 0)) {
            if (errorMessage)
                *errorMessage = tr("Qt version is too old.");
            return false;
        }
    } else {
        if (errorMessage)
            *errorMessage = tr("Device type is not desktop or Freebox.");
        return false;
    }

    return true;
}

Internal::Node *Project::rootProjectNode() const
{
    return static_cast<Internal::Node *>(ProjectExplorer::Project::rootProjectNode());
}

QStringList Project::files(ProjectExplorer::Project::FilesMode) const
{
    return files();
}

Project::RestoreResult Project::fromMap(const QVariantMap &map, QString *errorMessage)
{
    RestoreResult result = ProjectExplorer::Project::fromMap(map, errorMessage);
    if (result != RestoreResult::Ok)
        return result;

    // refresh first - project information is used e.g. to decide the default RC's
    refresh(RefreshEverything);

    updateKit();

    return RestoreResult::Ok;
}

bool Project::updateKit()
{
    using ProjectExplorer::Kit;
    using ProjectExplorer::KitManager;
    using ProjectExplorer::Target;

    QList<Kit*> kits = KitManager::matchingKits(
        ProjectExplorer::KitMatcher(std::function<bool(const Kit *)>([this](const Kit *k) -> bool {
            if (!k->isValid())
                return false;

            ProjectExplorer::IDevice::ConstPtr dev;
            dev = ProjectExplorer::DeviceKitInformation::device(k);
            if (dev.isNull())
                return false;

            if (dev->type() == Constants::FREEBOX_DEVICE_TYPE) {
                return true;
            }
            else if (dev->type() == ProjectExplorer::Constants::DESKTOP_DEVICE_TYPE) {
                QtSupport::BaseQtVersion *version = QtSupport::QtKitInformation::qtVersion(k);
                if (!version || version->type() != QLatin1String(QtSupport::Constants::DESKTOPQT))
                    return false;

                if (version->qtVersion() >= QtSupport::QtVersionNumber(5, 0, 0) &&
                                !version->qmlsceneCommand().isEmpty())
                    return true;
            }

            return false;
        })));

    foreach(Kit *kit, kits) {
        if (!target(kit))
            addTarget(createTarget(kit));
    }

    connect(this, &ProjectExplorer::Project::addedTarget, this, &Project::addedTarget);
    connect(this, &ProjectExplorer::Project::activeTargetChanged,
            this, &Project::onActiveTargetChanged);

    onActiveTargetChanged(activeTarget());

    return true;
}

} // namespace Freebox
