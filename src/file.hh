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
#ifndef PROJECTFILE_HH_
# define PROJECTFILE_HH_

#include <coreplugin/idocument.h>

namespace Freebox {

class Project;

namespace Internal {

class File : public Core::IDocument
{
    Q_OBJECT

public:

    File(Project *parent, const Utils::FileName &fileName);
    virtual ~File();

    virtual bool save(QString *errorString, const QString &fileName, bool autoSave);

    virtual QString defaultPath() const;
    virtual QString suggestedFileName() const;

    virtual bool isModified() const;
    virtual bool isSaveAsAllowed() const;

    ReloadBehavior reloadBehavior(ChangeTrigger state, ChangeType type) const;
    bool reload(QString *errorString, ReloadFlag flag, ChangeType type);

private:
    Project *m_project;
};

} // namespace Internal
} // namespace Freebox

#endif /* !PROJECTFILE_HH_ */
