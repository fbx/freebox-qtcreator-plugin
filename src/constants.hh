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
#ifndef FREEBOXCONSTANTS_H
#define FREEBOXCONSTANTS_H

#include <qmlprojectmanager/qmlprojectconstants.h>

namespace Freebox {
namespace Constants {

const char FREEBOX_DEVICE_TYPE[] = "Freebox";
const char FREEBOX_DEVICE_NT[] = "fbx:devel";

const char PROJECTCONTEXT[]     = "FbxProject.ProjectContext";
const char QML_VIEWER_RC_ID[] = "Freebox.QmlRunConfiguration";

const char FBXPROJECT_MIMETYPE[] = "application/x-fbxproject";

const char FBX_VIEWER_ARGUMENTS_KEY[] = "FbxProjectManager.RunConfiguration.QDeclarativeViewerArguments";
const char FBX_MAINSCRIPT_KEY[] = "FbxProjectManager.RunConfiguration.MainScript";

const char MAKE_PACKAGE_ACTION_ID[] = "Freebox.MakePackageAction";
const char MAKE_PACKAGE_AS_ACTION_ID[] = "Freebox.MakePackageAsAction";
const char MENU_ID[] = "Freebox.Menu";

} // namespace Freebox
} // namespace Constants

#endif // FREEBOXCONSTANTS_H

