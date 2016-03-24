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

#include "localrunconfiguration.hh"
#include "project.hh"
#include "constants.hh"
#include "environment.hh"
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/editormanager/ieditor.h>
#include <coreplugin/icore.h>
#include <coreplugin/idocument.h>
#include <projectexplorer/target.h>
#include <qtsupport/qtkitinformation.h>
#include <qtsupport/qtoutputformatter.h>
#include <qtsupport/qtsupportconstants.h>

#include <utils/fileutils.h>
#include <utils/mimetypes/mimedatabase.h>
#include <utils/qtcprocess.h>

namespace Freebox {

const char M_CURRENT_FILE[] = "CurrentFile";

LocalRunConfiguration::LocalRunConfiguration(ProjectExplorer::Target *parent, Core::Id id) :
    ProjectExplorer::RunConfiguration(parent, id)
{
    addExtraAspect(new EnvironmentAspect(this));

    ctor();
}

ProjectExplorer::Runnable LocalRunConfiguration::runnable() const
{
    ProjectExplorer::StandardRunnable r;
    r.executable = executable();
    r.commandLineArguments = commandLineArguments();
    r.runMode = ProjectExplorer::ApplicationLauncher::Gui;
    r.environment = extraAspect<EnvironmentAspect>()->environment();
    r.workingDirectory = canonicalCapsPath(target()->project()->projectFilePath()
                                           .toFileInfo().absolutePath());
    return r;
}

LocalRunConfiguration::LocalRunConfiguration(ProjectExplorer::Target *parent,
                                             LocalRunConfiguration *source) :
    ProjectExplorer::RunConfiguration(parent, source)
{
    ctor();
}

void
LocalRunConfiguration::ctor()
{
    setDisplayName(QString::fromUtf8(id().name()));
    updateEnabled();
}

QString LocalRunConfiguration::executable() const
{
    QtSupport::BaseQtVersion *version = qtVersion();
    if (!version)
        return QString();

    return version->qmlsceneCommand();
}

QString LocalRunConfiguration::commandLineArguments() const
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

/* QtDeclarative checks explicitly that the capitalization for any URL / path
   is exactly like the capitalization on disk.*/
QString LocalRunConfiguration::canonicalCapsPath(const QString &fileName)
{
    return Utils::FileUtils::normalizePathName(QFileInfo(fileName).canonicalFilePath());
}

QtSupport::BaseQtVersion *LocalRunConfiguration::qtVersion() const
{
    return QtSupport::QtKitInformation::qtVersion(target()->kit());
}

QWidget *LocalRunConfiguration::createConfigurationWidget()
{
    return 0;
}

Utils::OutputFormatter *LocalRunConfiguration::createOutputFormatter() const
{
    return new QtSupport::QtOutputFormatter(target()->project());
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
    if (source == FileInEditor) {
        m_scriptFile = QLatin1String(M_CURRENT_FILE);
        m_mainScriptFilename.clear();
    } else if (source == FileInProjectFile) {
        m_scriptFile.clear();
        m_mainScriptFilename.clear();
    } else { // FileInSettings
        m_scriptFile = settingsPath;
        m_mainScriptFilename
                = target()->project()->projectDirectory().toString() + QLatin1Char('/') + m_scriptFile;
    }
    updateEnabled();

    emit scriptSourceChanged();
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

    return ProjectExplorer::RunConfiguration::fromMap(map);
}

void LocalRunConfiguration::updateEnabled()
{
    Utils::MimeDatabase mdb;
    if (mainScriptSource() == FileInEditor) {
        Core::IDocument *document = Core::EditorManager::currentDocument();
        if (document) {
            m_currentFileFilename = document->filePath().toString();
        }
        if (!document
                || mdb.mimeTypeForFile(mainScript()).matchesName(QLatin1String("application/x-qmlproject"))) {
            // find a qml file with lowercase filename. This is slow, but only done
            // in initialization/other border cases.
            foreach (const QString &filename, target()->project()->files(ProjectExplorer::Project::AllFiles)) {
                const QFileInfo fi(filename);

                if (!filename.isEmpty() && fi.baseName()[0].isLower()
                        && mdb.mimeTypeForFile(fi).matchesName(QLatin1String("text/x-qml")))
                {
                    m_currentFileFilename = filename;
                    break;
                }
            }
        }
    }
}

} // namespace Freebox
