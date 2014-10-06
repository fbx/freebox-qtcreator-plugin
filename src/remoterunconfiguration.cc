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
#include <QDebug>

#include <coreplugin/mimedatabase.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/editormanager/ieditor.h>
#include <projectexplorer/target.h>
#include <qmlprojectmanager/qmlprojectmanagerconstants.h>

#include "constants.hh"
#include "remoterunconfiguration.hh"
#include "project.hh"

namespace Freebox {

const char M_CURRENT_FILE[] = "CurrentFile";

RemoteRunConfiguration::RemoteRunConfiguration(ProjectExplorer::Target *parent, Core::Id id) :
    ProjectExplorer::RunConfiguration(parent, id)
{
    ctor();
}

RemoteRunConfiguration::RemoteRunConfiguration(ProjectExplorer::Target *parent,
                                               RemoteRunConfiguration *source) :
    ProjectExplorer::RunConfiguration(parent, source)
{
    ctor();
}

QWidget *
RemoteRunConfiguration::createConfigurationWidget()
{
    return 0;
}

QString
RemoteRunConfiguration::workingDirectory() const
{
    return QString();
}

QString
RemoteRunConfiguration::commandLineArguments() const
{
    return QString();
}

void
RemoteRunConfiguration::ctor()
{
    setDisplayName(QString::fromUtf8(id().name()));
}

/**
  Returns absolute path to main script file.
  */
QString RemoteRunConfiguration::mainScript() const
{
    Project *project = qobject_cast<Project *>(target()->project());
    if (!project)
        return m_currentFileFilename;
    if (!project->mainFile().isEmpty()) {
        const QString pathInProject = project->mainFile();
        if (QFileInfo(pathInProject).isAbsolute())
            return pathInProject;
        else
            return project->projectDir().absoluteFilePath(pathInProject);
    }

    if (!m_mainScriptFilename.isEmpty())
        return m_mainScriptFilename;

    return m_currentFileFilename;
}

void RemoteRunConfiguration::setScriptSource(MainScriptSource source,
                                             const QString &settingsPath)
{
    Project *project = qobject_cast<Project *>(target()->project());

    QTC_ASSERT(project, return);

    if (source == FileInEditor) {
        m_scriptFile = QLatin1String(M_CURRENT_FILE);
        m_mainScriptFilename.clear();
    } else if (source == FileInProjectFile) {
        m_scriptFile.clear();
        m_mainScriptFilename.clear();
    } else { // FileInSettings
        m_scriptFile = settingsPath;
        m_mainScriptFilename = project->projectDir().canonicalPath()
                    + QLatin1Char('/') + m_scriptFile;
    }
    updateEnabled();

    emit scriptSourceChanged();
}

RemoteRunConfiguration::MainScriptSource RemoteRunConfiguration::mainScriptSource() const
{
    Project *project = static_cast<Project *>(target()->project());

    if (!project->mainFile().isEmpty())
        return FileInProjectFile;
    if (!m_mainScriptFilename.isEmpty())
        return FileInSettings;
    return FileInEditor;
}

void RemoteRunConfiguration::updateEnabled()
{
    if (mainScriptSource() == FileInEditor) {
        Core::IDocument *document = Core::EditorManager::currentDocument();
        if (document) {
            m_currentFileFilename = document->filePath();
        }
        if (!document
            || Core::MimeDatabase::findByFile(mainScript()).type() ==
                    QLatin1String("application/x-qmlproject")) {
            // find a qml file with lowercase filename. This is slow, but only done
            // in initialization/other border cases.
            foreach (const QString &filename, target()->project()->files(ProjectExplorer::Project::AllFiles)) {
                const QFileInfo fi(filename);

                if (!filename.isEmpty() && fi.baseName()[0].isLower()
                    && Core::MimeDatabase::findByFile(fi).type() ==
                        QLatin1String("application/x-qml"))
                {
                    m_currentFileFilename = filename;
                    break;
                }
            }
        }
    }
}

bool RemoteRunConfiguration::fromMap(const QVariantMap &map)
{
    m_qmlViewerArgs = map.value(QLatin1String(Constants::FBX_VIEWER_ARGUMENTS_KEY)).toString();
    m_scriptFile = map.value(QLatin1String(Constants::FBX_MAINSCRIPT_KEY), QLatin1String(M_CURRENT_FILE)).toString();

    if (m_scriptFile == QLatin1String(M_CURRENT_FILE))
        setScriptSource(FileInEditor);
    else if (m_scriptFile.isEmpty())
        setScriptSource(FileInProjectFile);
    else
        setScriptSource(FileInSettings, m_scriptFile);

    return ProjectExplorer::RunConfiguration::fromMap(map);
}

} // namespace Freebox