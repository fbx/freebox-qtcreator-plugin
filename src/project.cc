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
#include <QJsonObject>

#include <projectexplorer/projectexplorerconstants.h>
#include <coreplugin/icontext.h>
#include <coreplugin/documentmanager.h>
#include <coreplugin/messagemanager.h>
#include <qtsupport/baseqtversion.h>
#include <qtsupport/qtkitinformation.h>
#include <qtsupport/qtsupportconstants.h>
#include <projectexplorer/kitinformation.h>
#include <projectexplorer/kitmanager.h>
#include <projectexplorer/target.h>
#include <projectexplorer/buildtargetinfo.h>

#include "qmlprojectmanager/fileformat/qmlprojectfileformat.h"
#include "qmlprojectmanager/fileformat/qmlprojectitem.h"
#include "fileformat/manifest.hh"

#include "constants.hh"
#include "remoterunconfiguration.hh"
#include "localrunconfiguration.hh"
#include "manager.hh"
#include "project.hh"
#include "file.hh"
#include "node.hh"

namespace Freebox {

namespace Internal {

class ProjectKitMatcher : public ProjectExplorer::KitMatcher
{
public:

    ProjectKitMatcher() {}

    bool matches(const ProjectExplorer::Kit *k) const
    {
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
    }
};

} // namespace Internal

Project::Project(Internal::Manager *manager, const QString &fileName) :
    m_manager(manager),
    m_fileName(fileName),
    m_modelManager(QmlJS::ModelManagerInterface::instance()),
    m_activeTarget(0)
{
    setId("FbxProjectManager.FbxProject");
    setProjectContext(Core::Context(Constants::PROJECTCONTEXT));
    setProjectLanguages(Core::Context(ProjectExplorer::Constants::LANG_QMLJS));

    QFileInfo fileInfo(m_fileName);
    m_projectName = fileInfo.completeBaseName();

    m_file = new Internal::File(this, fileName);
    m_rootNode = new Internal::Node(this, m_file);
    Core::DocumentManager::addDocument(m_file, true);
    m_manager->registerProject(this);

    connect(ProjectExplorer::KitManager::instance(), SIGNAL(kitsChanged()),
            this, SLOT(updateKit()));
}

Project::~Project()
{
    m_manager->unregisterProject(this);

    Core::DocumentManager::removeDocument(m_file);

    delete m_projectItem.data();
    delete m_rootNode;
}

QString Project::filesFileName() const
{
    return m_fileName;
}

QString Project::displayName() const
{
    return m_projectName;
}

Core::IDocument *Project::document() const
{
    return m_file;
}

ProjectExplorer::IProjectManager *Project::projectManager() const
{
    return m_manager;
}

QStringList Project::files(ProjectExplorer::Project::FilesMode) const
{
    return files();
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

bool Project::updateKit()
{
    using ProjectExplorer::Kit;
    using ProjectExplorer::KitManager;
    using ProjectExplorer::Target;

    // find a kit that matches prerequisites (prefer default one)
    Internal::ProjectKitMatcher matcher;
    QList<Kit*> kits;

    connect(this, SIGNAL(addedTarget(ProjectExplorer::Target*)),
            SLOT(addedTarget(ProjectExplorer::Target*)));

    connect(this, SIGNAL(activeTargetChanged(ProjectExplorer::Target*)),
            SLOT(onActiveTargetChanged(ProjectExplorer::Target*)));

    kits = KitManager::matchingKits(matcher);

    foreach(Kit *kit, kits) {
        if (!target(kit)) {
            Target *t = createTarget(kit);
            addTarget(t);
        }
    }

    // make sure we get updates on kit changes
    m_activeTarget = activeTarget();
    if (m_activeTarget)
        connect(m_activeTarget, SIGNAL(kitChanged()), this, SLOT(onKitChanged()));

    return true;
}

bool Project::fromMap(const QVariantMap &map)
{
    if (!ProjectExplorer::Project::fromMap(map))
        return false;

    // refresh first - project information is used e.g. to decide the default RC's
    refresh(RefreshEverything);

    return updateKit();
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

QString Project::projectFilePathString(const QString &str) const
{
    return str;
}

QString Project::projectFilePathString(const Utils::FileName &filename) const
{
    return filename.toString();
}

QDir Project::projectDir() const
{
    return QFileInfo(projectFilePathString(projectFilePath())).dir();
}

ProjectExplorer::ProjectNode *Project::rootProjectNode() const
{
    return m_rootNode;
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

void Project::parseProject(RefreshOptions options)
{
    if (options & RefreshFiles) {
        if (options & RefreshProjectFile) {
            delete m_projectItem.data();
        }

        m_manifest = Fileformat::Manifest(projectDir().canonicalPath()
                                          + QString::fromAscii("/manifest.json"));

        if (!m_projectItem) {
            QString errorMessage;
            m_projectItem =
                    QmlProjectManager::QmlProjectFileFormat::parseProjectFile(m_fileName,
                                                                              &errorMessage);
            if (m_projectItem) {
                connect(m_projectItem, SIGNAL(qmlFilesChanged(QSet<QString>,QSet<QString>)),
                        this, SLOT(refreshFiles(QSet<QString>,QSet<QString>)));

            } else {
                Core::MessageManager::write(tr("Error while loading project file %1.").arg(m_fileName), Core::MessageManager::NoModeSwitch);
                Core::MessageManager::write(errorMessage);
            }
        }

        if (m_projectItem) {
            m_projectItem->setSourceDirectory(projectDir().path());
            m_modelManager->updateSourceFiles(m_projectItem->files(), true);

            QString mainFilePath = m_projectItem->mainFile();
            if (!mainFilePath.isEmpty()) {
                mainFilePath = projectDir().absoluteFilePath(mainFilePath);
                Utils::FileReader reader;
                QString errorMessage;
                if (!reader.fetch(mainFilePath, &errorMessage)) {
                    Core::MessageManager::write(tr("Warning while loading project file %1.").arg(m_fileName));
                    Core::MessageManager::write(errorMessage);
                }
            }
        }
        m_rootNode->refresh();
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
        m_rootNode->refresh();

    QmlJS::ModelManagerInterface::ProjectInfo projectInfo =
            QmlJSTools::defaultProjectInfoForProject(this);
    projectInfo.importPaths = customImportPaths();

    m_modelManager->updateProjectInfo(projectInfo, this);
}

void Project::refreshProjectFile()
{
    refresh(RefreshProjectFile | RefreshFiles);
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

void Project::refreshFiles(const QSet<QString> &/*added*/, const QSet<QString> &removed)
{
    refresh(RefreshFiles);
    if (!removed.isEmpty())
        m_modelManager->removeFiles(removed.toList());
}

void Project::addedTarget(ProjectExplorer::Target *target)
{
    connect(target, SIGNAL(addedRunConfiguration(ProjectExplorer::RunConfiguration*)),
            this, SLOT(addedRunConfiguration(ProjectExplorer::RunConfiguration*)));
    foreach (ProjectExplorer::RunConfiguration *rc, target->runConfigurations())
        addedRunConfiguration(rc);
}

void Project::onActiveTargetChanged(ProjectExplorer::Target *target)
{
    if (m_activeTarget)
        disconnect(m_activeTarget, SIGNAL(kitChanged()), this, SLOT(onKitChanged()));
    m_activeTarget = target;
    if (m_activeTarget)
        connect(target, SIGNAL(kitChanged()), this, SLOT(onKitChanged()));

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

} // namespace Freebox
