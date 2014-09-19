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
#include <QFileInfo>
#include <QStyle>

#include <coreplugin/idocument.h>
#include <coreplugin/fileiconprovider.h>
#include <projectexplorer/projectexplorer.h>

#include "project.hh"
#include "node.hh"

namespace Freebox {
namespace Internal {

Node::Node(Project *project, Core::IDocument *projectFile)
    : ProjectExplorer::ProjectNode(QFileInfo(projectFile->filePath()).absoluteFilePath()),
      m_project(project),
      m_projectFile(projectFile)
{
    setDisplayName(QFileInfo(projectFile->filePath()).completeBaseName());
    // make overlay
    const QSize desiredSize = QSize(16, 16);
    const QIcon projectBaseIcon(QLatin1String(":/freebox/images/qmlfolder.png"));
    const QPixmap projectPixmap = Core::FileIconProvider::overlayIcon(QStyle::SP_DirIcon,
                                                                      projectBaseIcon,
                                                                      desiredSize);
    setIcon(QIcon(projectPixmap));
}

Node::~Node()
{ }

Core::IDocument *Node::projectFile() const
{
    return m_projectFile;
}

QString Node::projectFilePath() const
{
    return m_projectFile->filePath();
}

void Node::refresh()
{
    using namespace ProjectExplorer;

    // remove the existing nodes.
    removeFileNodes(fileNodes());
    removeFolderNodes(subFolderNodes());

    //ProjectExplorerPlugin::instance()->setCurrentNode(0); // ### remove me

    FileNode *projectFilesNode = new FileNode(m_project->filesFileName(),
                                              ProjectFileType,
                                              /* generated = */ false);

    QStringList files = m_project->files();
    files.removeAll(m_project->filesFileName());

    addFileNodes(QList<FileNode *>()
                 << projectFilesNode);

    QHash<QString, QStringList> filesInDirectory;

    foreach (const QString &fileName, files) {
        QFileInfo fileInfo(fileName);

        QString absoluteFilePath;
        QString relativeDirectory;

        if (fileInfo.isAbsolute()) {
            // plain old file format
            absoluteFilePath = fileInfo.filePath();
            relativeDirectory = m_project->projectDir().relativeFilePath(fileInfo.path());
        } else {
            absoluteFilePath = m_project->projectDir().absoluteFilePath(fileInfo.filePath());
            relativeDirectory = fileInfo.path();
            if (relativeDirectory == QLatin1String("."))
                relativeDirectory.clear();
        }

        filesInDirectory[relativeDirectory].append(absoluteFilePath);
    }

    const QHash<QString, QStringList>::ConstIterator cend = filesInDirectory.constEnd();
    for (QHash<QString, QStringList>::ConstIterator it = filesInDirectory.constBegin(); it != cend; ++it) {
        FolderNode *folder = findOrCreateFolderByName(it.key());

        QList<FileNode *> fileNodes;
        foreach (const QString &file, it.value()) {
            FileType fileType = SourceType; // ### FIXME
            FileNode *fileNode = new FileNode(file, fileType, /*generated = */ false);
            fileNodes.append(fileNode);
        }

        folder->addFileNodes(fileNodes);
    }

    m_folderByName.clear();
}

ProjectExplorer::FolderNode *Node::findOrCreateFolderByName(const QStringList &components, int end)
{
    if (! end)
        return 0;

    QString baseDir = QFileInfo(path()).path();

    QString folderName;
    for (int i = 0; i < end; ++i) {
        folderName.append(components.at(i));
        folderName += QLatin1Char('/'); // ### FIXME
    }

    const QString component = components.at(end - 1);

    if (component.isEmpty())
        return this;

    else if (FolderNode *folder = m_folderByName.value(folderName))
        return folder;

    FolderNode *folder = new FolderNode(baseDir + QLatin1Char('/') + folderName);
    folder->setDisplayName(component);

    m_folderByName.insert(folderName, folder);

    FolderNode *parent = findOrCreateFolderByName(components, end - 1);
    if (! parent)
        parent = this;

    parent->addFolderNodes(QList<FolderNode*>() << folder);

    return folder;
}

ProjectExplorer::FolderNode *Node::findOrCreateFolderByName(const QString &filePath)
{
    QStringList components = filePath.split(QLatin1Char('/'));
    return findOrCreateFolderByName(components, components.length());
}

bool Node::showInSimpleTree() const
{
    return true;
}

QList<ProjectExplorer::ProjectAction> Node::supportedActions(ProjectExplorer::Node *node) const
{
    Q_UNUSED(node);
    QList<ProjectExplorer::ProjectAction> actions;
    actions.append(ProjectExplorer::AddNewFile);
    actions.append(ProjectExplorer::EraseFile);
    if (node->nodeType() == ProjectExplorer::FileNodeType) {
        ProjectExplorer::FileNode *fileNode = static_cast<ProjectExplorer::FileNode *>(node);
        if (fileNode->fileType() != ProjectExplorer::ProjectFileType)
            actions.append(ProjectExplorer::Rename);
    }
    return actions;
}

bool Node::canAddSubProject(const QString &proFilePath) const
{
    Q_UNUSED(proFilePath)
    return false;
}

bool Node::addSubProjects(const QStringList &proFilePaths)
{
    Q_UNUSED(proFilePaths)
    return false;
}

bool Node::removeSubProjects(const QStringList &proFilePaths)
{
    Q_UNUSED(proFilePaths)
    return false;
}

bool Node::addFiles(const QStringList &filePaths, QStringList * /*notAdded*/)
{
    return m_project->addFiles(filePaths);
}

bool Node::removeFiles(const QStringList & /*filePaths*/, QStringList * /*notRemoved*/)
{
    return false;
}

bool Node::deleteFiles(const QStringList & /*filePaths*/)
{
    return true;
}

bool Node::renameFile(const QString & /*filePath*/, const QString & /*newFilePath*/)
{
    return true;
}

QList<ProjectExplorer::RunConfiguration *> Node::runConfigurationsFor(ProjectExplorer::Node *node)
{
    Q_UNUSED(node)
    return QList<ProjectExplorer::RunConfiguration *>();
}

} // namespace Internal
} // namespace Freebox
