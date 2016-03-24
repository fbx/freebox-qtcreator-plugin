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
#ifndef NODE_HH_
# define NODE_HH_

#include <projectexplorer/projectnodes.h>

#include <QStringList>
#include <QHash>

namespace Core { class IDocument; }

namespace Freebox {

class Project;

namespace Internal {

class Node : public ProjectExplorer::ProjectNode
{
public:
    Node(Project *project);
    virtual ~Node();

    virtual bool showInSimpleTree() const;

    virtual QList<ProjectExplorer::ProjectAction> supportedActions(ProjectExplorer::Node *node) const;

    virtual bool addFiles(const QStringList &filePaths, QStringList *notAdded = 0);
    virtual bool deleteFiles(const QStringList &filePaths);
    virtual bool renameFile(const QString &filePath, const QString &newFilePath);

    void refresh();

private:
    FolderNode *findOrCreateFolderByName(const QString &filePath);
    FolderNode *findOrCreateFolderByName(const QStringList &components, int end);

    Project *m_project;
    QHash<QString, FolderNode *> m_folderByName;
};

} // namespace Internal
} // namespace Freebox


#endif /* !NODE_HH_ */
