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

#include "file.hh"
#include "project.hh"
#include "constants.hh"
#include <utils/qtcassert.h>

namespace Freebox {
namespace Internal {

File::File(Project *parent, const Utils::FileName &fileName)
    : Core::IDocument(parent),
      m_project(parent)
{
    QTC_CHECK(m_project);
    QTC_CHECK(!fileName.isEmpty());
    setId("Qml.ProjectFile");
    setMimeType(QLatin1String(Constants::FBXPROJECT_MIMETYPE));
    setFilePath(fileName);
}

bool File::save(QString *, const QString &, bool)
{
    return false;
}

QString File::defaultPath() const
{
    return QString();
}

QString File::suggestedFileName() const
{
    return QString();
}

bool File::isModified() const
{
    return false;
}

bool File::isSaveAsAllowed() const
{
    return false;
}

Core::IDocument::ReloadBehavior File::reloadBehavior(ChangeTrigger state, ChangeType type) const
{
    Q_UNUSED(state)
    Q_UNUSED(type)
    return BehaviorSilent;
}

bool File::reload(QString *errorString, ReloadFlag flag, ChangeType type)
{
    Q_UNUSED(errorString)
    Q_UNUSED(flag)

    if (type == TypeContents)
        m_project->refreshProjectFile();

    return true;
}

} // namespace Internal
} // namespace Freebox
