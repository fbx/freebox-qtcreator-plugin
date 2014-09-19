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
#ifndef FREEBOXWIZARD_H
#define FREEBOXWIZARD_H

#include <coreplugin/basefilewizardfactory.h>
#include <projectexplorer/baseprojectwizarddialog.h>

namespace ExtensionSystem { class IPlugin; }

namespace Freebox {
namespace Internal {

class App;
class TemplateInfo;
class QmlComponentSetPage;
class ProjectDescPage;

class WizardDialog : public ProjectExplorer::BaseProjectWizardDialog
{
    Q_OBJECT
public:
    WizardDialog(QWidget *parent,
                 const Core::WizardDialogParameters &parameters);

    TemplateInfo templateInfo() const;

    QString projectDesc() const;

private:
    QmlComponentSetPage *m_componentSetPage;
    ProjectDescPage *m_projectDescPage;
};

class Wizard : public Core::BaseFileWizardFactory
{
    Q_OBJECT

public:
    Wizard();

    static void createInstances(ExtensionSystem::IPlugin *plugin);

private:
    Core::BaseFileWizard *create(QWidget *parent, const Core::WizardDialogParameters &parameters) const;
    Core::GeneratedFiles generateFiles(const QWizard *w, QString *errorMessage) const;
    void writeUserFile(const QString &fileName) const;
    bool postGenerateFiles(const QWizard *w, const Core::GeneratedFiles &l, QString *errorMessage);

private:
    App *m_App;
};

} // namespace Internal
} // namespace Freebox

#endif // FREEBOXWIZARD_H
