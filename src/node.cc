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

#include "node.hh"
#include "project.hh"

#include <coreplugin/idocument.h>
#include <coreplugin/fileiconprovider.h>
#include <projectexplorer/projectexplorer.h>

#include <utils/algorithm.h>

#include <QFileInfo>
#include <QStyle>

namespace Freebox {
namespace Internal {

Node::Node(Project *project)
    : ProjectExplorer::ProjectNode(project->projectDirectory()),
      m_project(project)
{
    setDisplayName(project->projectFilePath().toFileInfo().completeBaseName());
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

void Node::refresh()
{
    using namespace ProjectExplorer;

    FileNode *projectFilesNode = new FileNode(m_project->filesFileName(),
                                              ProjectFileType,
                                              /* generated = */ false);

    QStringList files = m_project->files();
    files.removeAll(m_project->filesFileName().toString());

    QList<FileNode *> fileNodes = Utils::transform(files, [](const QString &f) {
        FileType fileType = SourceType; // ### FIXME
        return new FileNode(Utils::FileName::fromString(f), fileType, false);
    });
    fileNodes.append(projectFilesNode);

    buildTree(fileNodes);
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

bool Node::addFiles(const QStringList &filePaths, QStringList * /*notAdded*/)
{
    return m_project->addFiles(filePaths);
}

bool Node::deleteFiles(const QStringList & /*filePaths*/)
{
    return true;
}

bool Node::renameFile(const QString & /*filePath*/, const QString & /*newFilePath*/)
{
    return true;
}

} // namespace Internal
} // namespace Freebox
