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

#include <projectexplorer/environmentaspect.h>

namespace Freebox {

class EnvironmentAspect : public ProjectExplorer::EnvironmentAspect
{
    Q_OBJECT

public:
    EnvironmentAspect(ProjectExplorer::RunConfiguration *rc);
    EnvironmentAspect *create(ProjectExplorer::RunConfiguration *parent) const override;

    QList<int> possibleBaseEnvironments() const override;
    QString baseEnvironmentDisplayName(int base) const override;

    Utils::Environment baseEnvironment() const override;

private:
    enum BaseEnvironmentBase {
        SystemEnvironmentBase = 0
    };
};

} // namespace Freebox

