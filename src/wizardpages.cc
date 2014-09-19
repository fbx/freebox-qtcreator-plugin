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
#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QTextEdit>

#include "app.hh"
#include "wizardpages.hh"

namespace Freebox {
namespace Internal {

class QmlComponentSetPagePrivate
{
public:
    QComboBox *m_versionComboBox;
    QLabel *m_detailedDescriptionLabel;
};

QmlComponentSetPage::QmlComponentSetPage(QWidget *parent)
    : QWizardPage(parent)
    , d(new QmlComponentSetPagePrivate)
{
    setTitle(tr("Select Qt Quick Component Set"));
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *l = new QHBoxLayout();

    QLabel *label = new QLabel(tr("Qt Quick component set:"), this);
    d->m_versionComboBox = new QComboBox(this);

    foreach (const TemplateInfo &templateInfo, App::templateInfos())
        d->m_versionComboBox->addItem(templateInfo.displayName);

    l->addWidget(label);
    l->addWidget(d->m_versionComboBox);

    d->m_detailedDescriptionLabel = new QLabel(this);
    d->m_detailedDescriptionLabel->setWordWrap(true);
    d->m_detailedDescriptionLabel->setTextFormat(Qt::RichText);
    connect(d->m_versionComboBox, SIGNAL(currentIndexChanged(int)), this,
            SLOT(updateDescription(int)));
    updateDescription(d->m_versionComboBox->currentIndex());

    mainLayout->addLayout(l);
    mainLayout->addWidget(d->m_detailedDescriptionLabel);
}

QmlComponentSetPage::~QmlComponentSetPage()
{
    delete d;
}

TemplateInfo QmlComponentSetPage::templateInfo() const
{
    if (App::templateInfos().isEmpty())
        return TemplateInfo();
    return App::templateInfos().at(d->m_versionComboBox->currentIndex());
}

void QmlComponentSetPage::updateDescription(int index)
{
    if (App::templateInfos().isEmpty())
        return;

    const TemplateInfo templateInfo = App::templateInfos().at(index);
    d->m_detailedDescriptionLabel->setText(templateInfo.description);
}

ProjectDescPage::ProjectDescPage(QWidget *parent) :
    QWizardPage(parent)
{
    setTitle(QStringLiteral("Project description"));
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    {
        QHBoxLayout *l = new QHBoxLayout();
        QLabel *label = new QLabel(tr("Description:"), this);
        QLineEdit *edit = new QLineEdit(this);
        QValidator *validator = new QRegExpValidator(QRegExp(QStringLiteral("\\w[\\w\\s]+")), this);
        edit->setValidator(validator);
        l->addWidget(label);
        l->addWidget(edit);
        mainLayout->addLayout(l);

        mDescEdit = edit;
    }
}

QString ProjectDescPage::desc() const
{
    return mDescEdit->text();
}

} // namespace Internal
} // namespace Freebox
