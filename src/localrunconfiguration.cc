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

#include <utils/fileutils.h>
#include <utils/qtcprocess.h>
#include <coreplugin/mimedatabase.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/editormanager/ieditor.h>
#include <qtsupport/qtsupportconstants.h>
#include <qtsupport/qtkitinformation.h>
#include <qtsupport/qtoutputformatter.h>
#include <projectexplorer/target.h>
#include <projectexplorer/localapplicationrunconfiguration.h>
#include <qmlprojectmanager/qmlprojectmanagerconstants.h>
#include <qmlprojectmanager/qmlprojectenvironmentaspect.h>

#include "constants.hh"
#include "localrunconfiguration.hh"
#include "environment.hh"
#include "project.hh"

namespace Freebox {

const char M_CURRENT_FILE[] = "CurrentFile";

LocalRunConfiguration::LocalRunConfiguration(ProjectExplorer::Target *parent, Core::Id id) :
    ProjectExplorer::LocalApplicationRunConfiguration(parent, id)
{
    addExtraAspect(new EnvironmentAspect(this));

    ctor();
}

LocalRunConfiguration::LocalRunConfiguration(ProjectExplorer::Target *parent,
                                             LocalRunConfiguration *source) :
    ProjectExplorer::LocalApplicationRunConfiguration(parent, source)
{
    ctor();
}

QWidget *
LocalRunConfiguration::createConfigurationWidget()
{
    return 0;
}

QString
LocalRunConfiguration::workingDirectory() const
{
    return QString();
}

QString LocalRunConfiguration::canonicalCapsPath(const QString &fileName) const
{
    return Utils::FileUtils::normalizePathName(QFileInfo(fileName).canonicalFilePath());
}

QString
LocalRunConfiguration::commandLineArguments() const
{
    QString args;

    // arguments from .qmlproject file
    Project *project = static_cast<Project *>(target()->project());
    Fileformat::Manifest &m = project->manifest();
    QString file;

    file = m.entryPointFile(QString::fromUtf8(id().name()));

    if (file.isNull())
        return args;

    foreach (const QString &importPath, project->customImportPaths()) {
        Utils::QtcProcess::addArg(&args, QLatin1String("-I"));
        Utils::QtcProcess::addArg(&args, importPath);
    }

    QString s = project->projectDir().canonicalPath() + QStringLiteral("/") + file;
    if (!s.isEmpty()) {
        s = canonicalCapsPath(s);
        Utils::QtcProcess::addArg(&args, s);
    }
    return args;
}

void
LocalRunConfiguration::ctor()
{
    setDisplayName(QString::fromUtf8(id().name()));
}

QtSupport::BaseQtVersion *LocalRunConfiguration::qtVersion() const
{
    return QtSupport::QtKitInformation::qtVersion(target()->kit());
}

QString LocalRunConfiguration::executable() const
{
    QtSupport::BaseQtVersion *version = qtVersion();
    if (!version)
        return QString();

    return version->qmlsceneCommand();
}

ProjectExplorer::ApplicationLauncher::Mode
LocalRunConfiguration::runMode() const
{
    return ProjectExplorer::ApplicationLauncher::Gui;
}

/**
  Returns absolute path to main script file.
  */
QString LocalRunConfiguration::mainScript() const
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

void LocalRunConfiguration::setScriptSource(MainScriptSource source,
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
        m_mainScriptFilename
                        = project->projectDir().canonicalPath()
                        + QLatin1Char('/') + m_scriptFile;
    }
    updateEnabled();

    emit scriptSourceChanged();
}

LocalRunConfiguration::MainScriptSource LocalRunConfiguration::mainScriptSource() const
{
    Project *project = static_cast<Project *>(target()->project());

    if (!project->mainFile().isEmpty())
        return FileInProjectFile;
    if (!m_mainScriptFilename.isEmpty())
        return FileInSettings;
    return FileInEditor;
}

void LocalRunConfiguration::updateEnabled()
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

bool LocalRunConfiguration::fromMap(const QVariantMap &map)
{
    m_qmlViewerArgs = map.value(QLatin1String(Constants::FBX_VIEWER_ARGUMENTS_KEY)).toString();
    m_scriptFile = map.value(QLatin1String(Constants::FBX_MAINSCRIPT_KEY), QLatin1String(M_CURRENT_FILE)).toString();

    if (m_scriptFile == QLatin1String(M_CURRENT_FILE))
        setScriptSource(FileInEditor);
    else if (m_scriptFile.isEmpty())
        setScriptSource(FileInProjectFile);
    else
        setScriptSource(FileInSettings, m_scriptFile);

    return ProjectExplorer::LocalApplicationRunConfiguration::fromMap(map);
}

} // namespace Freebox
