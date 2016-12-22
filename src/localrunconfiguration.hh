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
#pragma once

# include <qtsupport/baseqtversion.h>
# include <projectexplorer/runnables.h>

namespace Freebox {

namespace Internal {
    class RunConfigurationFactory;
}

class LocalRunConfiguration : public ProjectExplorer::RunConfiguration
{
    Q_OBJECT

    friend class Internal::RunConfigurationFactory;
    friend class Project; // to call updateEnabled()

public:
    LocalRunConfiguration(ProjectExplorer::Target *parent, Core::Id id);

    ProjectExplorer::Runnable runnable() const override;

    QtSupport::BaseQtVersion *qtVersion() const;

    enum MainScriptSource {
        FileInEditor,
        FileInProjectFile,
        FileInSettings
    };
    MainScriptSource mainScriptSource() const;
    void setScriptSource(MainScriptSource source, const QString &settingsPath = QString());
    QString mainScript() const;

    QWidget *createConfigurationWidget() override;
    Utils::OutputFormatter *createOutputFormatter() const override;

signals:
    void scriptSourceChanged();

private slots:
    void updateEnabled();

protected:
    LocalRunConfiguration(ProjectExplorer::Target *parent,
                          LocalRunConfiguration *source);
    bool fromMap(const QVariantMap &map) override;
    void setEnabled(bool value);

private:
    void ctor();

    QString executable() const;
    QString commandLineArguments() const;

    static QString canonicalCapsPath(const QString &filePath);

    // absolute path to current file (if being used)
    QString m_currentFileFilename;
    // absolute path to selected main script (if being used)
    QString m_mainScriptFilename;

    QString m_scriptFile;
    QString m_qmlViewerArgs;

    bool m_isEnabled;
};

} // namespace Freebox

