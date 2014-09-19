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
#include <extensionsystem/pluginmanager.h>
#include <projectexplorer/customwizard/customwizard.h>
#include <projectexplorer/kitmanager.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <qmakeprojectmanager/qmakeproject.h>
#include <qmakeprojectmanager/qmakeprojectmanagerconstants.h>
#include <qtsupport/qtkitinformation.h>
#include <qtsupport/qtsupportconstants.h>

#include <QIcon>

#include "app.hh"
#include "wizard.hh"
#include "wizardpages.hh"

using namespace Core;
using namespace ExtensionSystem;
using namespace ProjectExplorer;
using namespace QmakeProjectManager;

namespace Freebox {
namespace Internal {

WizardDialog::WizardDialog(QWidget *parent, const WizardDialogParameters &parameters) :
    BaseProjectWizardDialog(parent, parameters)
{
    setWindowTitle(tr("New Freebox Project"));
    setIntroDescription(tr("This wizard generates a Freebox project."));
    m_componentSetPage = new QmlComponentSetPage;
    addPage(m_componentSetPage);

    m_projectDescPage = new ProjectDescPage;
    addPage(m_projectDescPage);
}

TemplateInfo WizardDialog::templateInfo() const
{
    return m_componentSetPage->templateInfo();
}

QString WizardDialog::projectDesc() const
{
    return m_projectDescPage->desc();
}


Wizard::Wizard() :
        m_App(new App(this))
{
    setWizardKind(ProjectWizard);
    setCategory(QLatin1String(ProjectExplorer::Constants::QT_APPLICATION_WIZARD_CATEGORY));
    setId(QLatin1String("QA.QMLC Application"));
    setIcon(QIcon(QLatin1String(QmakeProjectManager::Constants::ICON_QTQUICK_APP)));
    setDisplayCategory(QLatin1String(ProjectExplorer::Constants::QT_APPLICATION_WIZARD_CATEGORY_DISPLAY));
    setDisplayName(tr("Freebox"));
    setDescription(tr("Creates a Freebox project."));
    setRequiredFeatures(Core::Feature(QtSupport::Constants::FEATURE_QMLPROJECT)
                      | Core::Feature(QtSupport::Constants::FEATURE_QT_QUICK));
}

Core::BaseFileWizard *Wizard::create(QWidget *parent, const WizardDialogParameters &parameters) const
{
    WizardDialog *wizardDialog = new WizardDialog(parent, parameters);

    connect(wizardDialog, SIGNAL(projectParametersChanged(QString,QString)), m_App,
            SLOT(setProjectNameAndBaseDirectory(QString,QString)));

    wizardDialog->setPath(parameters.defaultPath());

    wizardDialog->setProjectName(WizardDialog::uniqueProjectName(parameters.defaultPath()));

    foreach (QWizardPage *page, parameters.extensionPages())
        wizardDialog->addPage(page);

    return wizardDialog;
}

GeneratedFiles Wizard::generateFiles(const QWizard *w,
                                     QString *errorMessage) const
{
    const WizardDialog *wizard = qobject_cast<const WizardDialog*>(w);
    m_App->setTemplateInfo(wizard->templateInfo());
    m_App->addReplacementVariable(QLatin1String("projectDesc"), wizard->projectDesc());
    return m_App->generateFiles(errorMessage);
}

bool Wizard::postGenerateFiles(const QWizard * /*wizard*/,
                               const GeneratedFiles &l,
                               QString *errorMessage)
{
    return ProjectExplorer::CustomProjectWizard::postGenerateOpen(l, errorMessage);
}

} // namespace Internal
} // namespace Freebox
