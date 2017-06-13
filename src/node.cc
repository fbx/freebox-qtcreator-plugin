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

bool Node::showInSimpleTree() const
{
    return true;
}

bool Node::supportsAction(ProjectExplorer::ProjectAction action, ProjectExplorer::Node *node) const
{
    using namespace ProjectExplorer;

    if (action == AddNewFile || action == EraseFile)
        return true;

    if (action == Rename && node->nodeType() == NodeType::File) {
        FileNode *fileNode = static_cast<FileNode *>(node);
        return fileNode->fileType() != FileType::Project;
    }

    return false;
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
