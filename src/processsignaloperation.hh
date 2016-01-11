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
#ifndef PROCESSSIGNALOPERATION_HH_
# define PROCESSSIGNALOPERATION_HH_

#include <projectexplorer/devicesupport/idevice.h>

namespace Freebox {

class FreeboxProcessSignalOperation :
    public ProjectExplorer::DeviceProcessSignalOperation
{
    Q_OBJECT

public:
    ~FreeboxProcessSignalOperation() {}
    void killProcess(qint64 pid);
    void killProcess(const QString &filePath);
    void interruptProcess(qint64 pid);
    void interruptProcess(const QString &filePath);
};

} // namespace Freebox

#endif /* !PROCESSSIGNALOPERATION_HH_ */
