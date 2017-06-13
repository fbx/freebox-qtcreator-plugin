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

#pragma once

#include <QObject>
#include <QSet>
#include <QStringList>

namespace QmlProjectManager {

class QmlProjectContentItem : public QObject {
    // base class for all elements that should be direct children of Project element
    Q_OBJECT

public:
    QmlProjectContentItem(QObject *parent = 0) : QObject(parent) {}
};

class QmlProjectItem : public QObject
{
    Q_OBJECT

public:
    QString sourceDirectory() const { return m_sourceDirectory; }
    void setSourceDirectory(const QString &directoryPath);

    QStringList importPaths() const { return m_absoluteImportPaths; }
    void setImportPaths(const QStringList &paths);

    QStringList files() const;
    bool matchesFile(const QString &filePath) const;

    QString mainFile() const { return m_mainFile; }
    void setMainFile(const QString &mainFilePath) { m_mainFile = mainFilePath; }

    void appendContent(QmlProjectContentItem* item) { m_content.append(item); }

signals:
    void qmlFilesChanged(const QSet<QString> &, const QSet<QString> &);

protected:
    QString m_sourceDirectory;
    QStringList m_importPaths;
    QStringList m_absoluteImportPaths;
    QString m_mainFile;
    QList<QmlProjectContentItem *> m_content; // content property
};

} // namespace QmlProjectManager

