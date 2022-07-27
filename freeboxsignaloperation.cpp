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
#include "freeboxsignaloperation.h"

namespace Freebox {

FreeboxSignalOperation::FreeboxSignalOperation()
{
}

FreeboxSignalOperation::~FreeboxSignalOperation() = default;

void FreeboxSignalOperation::killProcess(qint64 pid)
{
    Q_UNUSED(pid);
    m_errorMessage = QLatin1String("The freebox signal operation does "
                                   "not support killing by pid.");
    emit finished(m_errorMessage);
}

void FreeboxSignalOperation::killProcess(const QString &filePath)
{
    Q_UNUSED(filePath);
    m_errorMessage = QLatin1String("The freebox signal operation does "
                                   "not support killing by filepath.");
    emit finished(m_errorMessage);
}

void FreeboxSignalOperation::interruptProcess(qint64 pid)
{
    Q_UNUSED(pid);
    m_errorMessage = QLatin1String("The freebox signal operation does "
                                   "not support interrupting by pid.");
    emit finished(m_errorMessage);
}

void FreeboxSignalOperation::interruptProcess(const QString &filePath)
{
    Q_UNUSED(filePath);
    m_errorMessage = QLatin1String("The freebox signal operation does "
                                   "not support interrupting by filepath.");
    emit finished(m_errorMessage);
}

} // namespace Freebox
