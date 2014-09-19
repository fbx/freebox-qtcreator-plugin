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
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

#include "manifest.hh"

namespace Freebox {
namespace Fileformat {

Manifest::Manifest()
{
}

Manifest::Manifest(const QString &file) :
    m_filename(file)
{
    read();
}

void Manifest::read()
{
    QFile file(m_filename);

    if (!file.exists()) {
        qWarning() << QString::fromAscii("file %1 does not exists.").arg(m_filename);
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << QString::fromAscii("fail to open %1.").arg(m_filename);
        return;
    }

    QByteArray data = file.readAll();

    m_json = QJsonDocument::fromJson(data);
}

bool Manifest::isValid() const
{
    if (m_json.isNull())
        return false;

    if (!m_json.isObject())
        return false;

    return true;
}

QJsonValue Manifest::value(const QString &name) const
{
    if (!isValid()) {
        qWarning() << QString::fromAscii("Invalid Json manifest %1.").arg(m_filename);
        return QJsonValue();
    }

    return m_json.object()[name];
}

QString Manifest::stringValue(const QString &name) const
{
    QJsonValue v = value(name);

    if (v.type() != QJsonValue::String)
        return QString();

    return v.toString();
}

QStringList Manifest::entryPoints() const
{
    return value(QStringLiteral("entryPoints")).toObject().keys();
}

QString Manifest::entryPointFile(const QString &entry) const
{
    return value(QStringLiteral("entryPoints")).toObject()[entry].toObject()[QStringLiteral("file")].toString();
}

QString Manifest::name() const
{
    return stringValue(QStringLiteral("name"));
}

QString Manifest::identifier() const
{
    return stringValue(QStringLiteral("identifier"));
}

QString Manifest::description() const
{
    return stringValue(QStringLiteral("description"));
}

} // namespace Fileformat
} // namespace Freebox
