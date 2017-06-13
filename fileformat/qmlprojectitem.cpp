/****************************************************************************
 **
 ** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
 ** Contact: http://www.qt-project.org/legal
 **
 ** This file is part of Qt Creator.
 **
 ** Commercial License Usage
 ** Licensees holding valid commercial Qt licenses may use this file in
 ** accordance with the commercial license agreement provided with the
 ** Software or, alternatively, in accordance with the terms contained in
 ** a written agreement between you and Digia.  For licensing terms and
 ** conditions see http://qt.digia.com/licensing.  For further information
 ** use the contact form at http://qt.digia.com/contact-us.
 **
 ** GNU Lesser General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU Lesser
 ** General Public License version 2.1 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.LGPL included in the
 ** packaging of this file.  Please review the following information to
 ** ensure the GNU Lesser General Public License version 2.1 requirements
 ** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
 **
 ** In addition, as a special exception, Digia gives you certain additional
 ** rights.  These rights are described in the Digia Qt LGPL Exception
 ** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
 **
 ****************************************************************************/

#include "qmlprojectitem.hh"
#include "filefilteritems.hh"

#include <QDir>

namespace QmlProjectManager {

// kind of initialization
void QmlProjectItem::setSourceDirectory(const QString &directoryPath)
{
    if (m_sourceDirectory == directoryPath)
        return;

    m_sourceDirectory = directoryPath;

    for (int i = 0; i < m_content.size(); ++i) {
        QmlProjectContentItem *contentElement = m_content.at(i);
        FileFilterBaseItem *fileFilter = qobject_cast<FileFilterBaseItem*>(contentElement);
        if (fileFilter) {
            fileFilter->setDefaultDirectory(directoryPath);
            connect(fileFilter, &FileFilterBaseItem::filesChanged,
                    this, &QmlProjectItem::qmlFilesChanged);
        }
    }

    setImportPaths(m_importPaths);
}

void QmlProjectItem::setImportPaths(const QStringList &importPaths)
{
    if (m_importPaths != importPaths)
        m_importPaths = importPaths;

    // convert to absolute paths
    QStringList absoluteImportPaths;
    const QDir sourceDir(sourceDirectory());
    foreach (const QString &importPath, importPaths)
        absoluteImportPaths += QDir::cleanPath(sourceDir.absoluteFilePath(importPath));

    if (m_absoluteImportPaths == absoluteImportPaths)
        return;

    m_absoluteImportPaths = absoluteImportPaths;
}

/* Returns list of absolute paths */
QStringList QmlProjectItem::files() const
{
    QStringList files;

    for (QmlProjectContentItem *contentElement : m_content) {
        if (auto fileFilter = qobject_cast<FileFilterBaseItem*>(contentElement)) {
            foreach (const QString &file, fileFilter->files()) {
                if (!files.contains(file))
                    files << file;
            }
        }
    }
    return files;
}

/**
  Check whether the project would include a file path
  - regardless whether the file already exists or not.

  @param filePath: absolute file path to check
  */
bool QmlProjectItem::matchesFile(const QString &filePath) const
{
    for (QmlProjectContentItem *contentElement : m_content) {
        if (auto fileFilter = qobject_cast<FileFilterBaseItem*>(contentElement)) {
            if (fileFilter->matchesFile(filePath))
                return true;
        }
    }
    return false;
}

} // namespace QmlProjectManager
