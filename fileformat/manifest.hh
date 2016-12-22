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

# include <QObject>
# include <QJsonDocument>
# include <QJsonObject>
# include <QJsonValue>
# include <QJsonArray>

#define MAN_ASSERT(cond, action) if (cond) {} else { action; } do {} while (0)
#define MAN_ASSERT_ERR(cond, action, error) MAN_ASSERT(cond, { m_lastError = error; action; })

namespace Freebox {
namespace Fileformat {

class Manifest
{
public:
    Manifest() { m_isValid = false; }
    Manifest(const QString &file);
    Manifest(const QJsonDocument &jsonDoc);

    QString name() const;
    QString identifier() const;
    QString description() const;
    QStringList entryPoints() const;
    QString entryPointFile(const QString &entry) const;

    QStringList &files() { return m_files; }
    QString &defaultEntryPoint() { return m_defaultEntryPoint; }

    bool isValid() { return m_isValid; }
    QString &lastError() { return m_lastError; }

private:
    bool read();
    QJsonValue value(const QString &name, const QJsonObject &obj) const;
    QJsonValue value(const QString &name) const { return value(name, m_obj); }

    bool isStringValue(const QString &name, const QJsonObject &obj) const;
    bool isStringValue(const QString &name) const { return isStringValue(name, m_obj); }

    QString stringValue(const QString &name, const QJsonObject &obj) const;
    QString stringValue(const QString &name) const { return stringValue(name, m_obj); }

    bool isNumberValue(const QString &name, const QJsonObject &obj) const;
    bool isNumberValue(const QString &name) const { return isNumberValue(name, m_obj); }

    int intValue(const QString &name, const QJsonObject &obj) const;
    int intValue(const QString &name) const { return intValue(name, m_obj); }

    QJsonObject objectValue(const QString &name, const QJsonObject &obj) const;
    QJsonObject objectValue(const QString &name) const { return objectValue(name, m_obj); }

    bool entryPointsIsValid() const;
    bool entryPointsValueIsValid(const QString &entry) const;
    bool isEntryPointsFileValid(const QString &entry) const;
    QString entryPointsFile(const QString &entry) const;
    bool isEntryPointsUrlHandlerValid(const QString &entry) const;
    QJsonArray entryPointsUrlHandler(const QString &entry) const;
    QString getString(const QString &key) const;
    bool checkRequiredProperty(QJsonObject &obj, const QString &key, const QString &where);
    bool checkNumberProperty(QJsonObject &obj, const QString &key, int min, int max, const QString &where);
    bool checkRegExpProperty(QJsonObject &obj, QString &key, QString &where);
    bool checkStringProperty(QJsonObject &obj, const QString &key, const QString &regExp, const QString &where);
    bool check();

    QString m_filename;
    QJsonDocument m_json;
    QJsonObject m_obj;

    QString m_lastError;
    bool m_isValid;
    int m_nbDefaults;

    QString m_defaultEntryPoint;
    QStringList m_files;
};

} // namespace Fileformat
} // namespace Freebox

