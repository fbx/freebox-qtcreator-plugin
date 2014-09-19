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
#ifndef PROJECT_HH_
# define PROJECT_HH_

#include <QFlag>
#include <QPointer>
#include <qglobal.h>
#include <utils/fileutils.h>
#include <projectexplorer/project.h>
#include "fileformat/manifest.hh"
#include <qmljstools/qmljsmodelmanager.h>

namespace ProjectExplorer { class RunConfiguration; }
namespace QmlProjectManager { class QmlProjectItem; }
namespace QmlJS { class ModelManagerInterface; }

namespace Freebox {

class Item;

namespace Internal {
class Manager;
class File;
class Node;
}

class Project : public ProjectExplorer::Project
{
    Q_OBJECT

public:

    enum RefreshOption {
        RefreshProjectFile   = 0x01,
        RefreshFiles         = 0x02,
        RefreshConfiguration = 0x04,
        RefreshEverything    = RefreshProjectFile |
                               RefreshFiles |
                               RefreshConfiguration
    };
    Q_DECLARE_FLAGS(RefreshOptions,RefreshOption)

    Project(Internal::Manager *manager, const QString &fileName);
    ~Project();

    QString filesFileName() const;
    QString displayName() const;
    Core::IDocument *document() const;
    ProjectExplorer::IProjectManager *projectManager() const;
    QStringList files(ProjectExplorer::Project::FilesMode mode) const;
    QStringList files() const;
    bool fromMap(const QVariantMap &map);
    QString mainFile() const;
    bool validProjectFile() const;
    QStringList customImportPaths() const;
    QDir projectDir() const;
    ProjectExplorer::ProjectNode *rootProjectNode() const;
    void parseProject(RefreshOptions options);
    bool addFiles(const QStringList &filePaths);
    void refresh(RefreshOptions options);
    void refreshProjectFile();
    bool supportsKit(ProjectExplorer::Kit *k, QString *errorMessage) const;
    Fileformat::Manifest &manifest() { return m_manifest; }
    QString packageFileName() { return m_packageFileName; }
    void setPackageFileName(QString fn) { m_packageFileName = fn; }

private slots:
    void refreshFiles(const QSet<QString> &added, const QSet<QString> &removed);
    void addedTarget(ProjectExplorer::Target *target);
    void onActiveTargetChanged(ProjectExplorer::Target *target);
    void onKitChanged();
    void addedRunConfiguration(ProjectExplorer::RunConfiguration *);
    bool updateKit();

private:
    QString projectFilePathString(const QString &str) const;
    QString projectFilePathString(const Utils::FileName &filename) const;

    QPointer<QmlProjectManager::QmlProjectItem> m_projectItem;
    Internal::Node *m_rootNode;
    Internal::Manager *m_manager;
    QString m_fileName;
    QString m_packageFileName;
    QString m_projectName;
    Internal::File *m_file;
    QStringList m_files;
    QmlJS::ModelManagerInterface *m_modelManager;
    ProjectExplorer::Target *m_activeTarget;
    Fileformat::Manifest m_manifest;
    QStringList m_entryPoints;
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(Freebox::Project::RefreshOptions)

#endif /* !PROJECT_HH_ */
