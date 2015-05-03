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
#ifndef FREEBOX_H
#define FREEBOX_H

#include "freebox_global.hh"

#include <extensionsystem/iplugin.h>
#include <projectexplorer/projectexplorer.h>

namespace Freebox {
namespace Internal {

class FreeboxPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "Freebox.json")

public:
    FreeboxPlugin();
    ~FreeboxPlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();
private slots:
    void makeFreeStorePackageAsAction();
    void makeFreeStorePackageAction();
    void onCurrentProjectChanged(ProjectExplorer::Project *project);

private:
    void makeFreeStorePackage(bool saveAs);

    QAction *m_actionMakePackage;
    QAction *m_actionMakePackageAs;
    QString m_outFileName;
};

} // namespace Internal
} // namespace Freebox

#endif // FREEBOX_H
