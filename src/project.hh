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

#include "fileformat/manifest.hh"
#include "manager.hh"
#include "node.hh"

#include <utils/fileutils.h>
#include <projectexplorer/project.h>

#include <QFlag>
#include <QPointer>
#include <qglobal.h>

namespace ProjectExplorer { class RunConfiguration; }
namespace QmlProjectManager { class QmlProjectItem; }
namespace QmlJS { class ModelManagerInterface; }

namespace Freebox {

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

    Project(Internal::Manager *manager, const Utils::FileName &filename);
    ~Project();

    Utils::FileName filesFileName() const;

    QString displayName() const;
    Internal::Manager *projectManager() const;
    QStringList files(ProjectExplorer::Project::FilesMode mode) const;
    QStringList files() const;
    RestoreResult fromMap(const QVariantMap &map, QString *errorMessage);
    QString mainFile() const;
    bool validProjectFile() const;
    QStringList customImportPaths() const;
    QDir projectDir() const;
    Internal::Node *rootProjectNode() const;
    void parseProject(RefreshOptions options);
    QmlJS::ModelManagerInterface *modelManager() const;
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
    QString m_packageFileName;
    QString m_projectName;
    QStringList m_files;
    ProjectExplorer::Target *m_activeTarget = 0;
    Fileformat::Manifest m_manifest;
    QStringList m_entryPoints;
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(Freebox::Project::RefreshOptions)

#endif /* !PROJECT_HH_ */
