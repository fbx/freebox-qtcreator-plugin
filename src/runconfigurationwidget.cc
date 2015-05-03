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
#include <QLineEdit>
#include <QComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QStandardItemModel>

#include <utils/detailswidget.h>
#include <projectexplorer/projectexplorer.h>

#include "runconfigurationwidget.hh"

using namespace Freebox;

FreeboxRunConfigurationWidget::FreeboxRunConfigurationWidget(FreeboxRunConfiguration *rc) :
    QWidget(0),
    m_fileListCombo(0),
    m_fileListModel(new QStandardItemModel(this))
{
    Q_UNUSED(rc);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);

    //
    // Qt Version, Arguments
    //

    Utils::DetailsWidget *detailsWidget = new Utils::DetailsWidget();
    detailsWidget->setState(Utils::DetailsWidget::NoSummary);

    QWidget *formWidget = new QWidget(detailsWidget);
    detailsWidget->setWidget(formWidget);
    QFormLayout *form = new QFormLayout(formWidget);
    form->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    m_fileListCombo = new QComboBox;
    m_fileListCombo->setModel(m_fileListModel);

    connect(m_fileListCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
            this, &FreeboxRunConfigurationWidget::setMainScript);
    connect(ProjectExplorer::ProjectExplorerPlugin::instance(), SIGNAL(fileListChanged()),
            SLOT(updateFileComboBox()));

    QLineEdit *qmlViewerArgs = new QLineEdit;
    //qmlViewerArgs->setText(rc->m_qmlViewerArgs);
    connect(qmlViewerArgs, &QLineEdit::textChanged,
            this, &FreeboxRunConfigurationWidget::onViewerArgsChanged);

    form->addRow(tr("Arguments:"), qmlViewerArgs);
    form->addRow(tr("Main QML file:"), m_fileListCombo);

    layout->addWidget(detailsWidget);

    //updateFileComboBox();

//    connect(rc, SIGNAL(scriptSourceChanged()),
//            this, SLOT(updateFileComboBox()));
}

void FreeboxRunConfigurationWidget::updateFileComboBox()
{
}

void FreeboxRunConfigurationWidget::setMainScript(int index)
{
    Q_UNUSED(index);
}

void FreeboxRunConfigurationWidget::onViewerArgsChanged()
{
}
