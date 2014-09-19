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

#include <QFile>
#include <QString>
#include <QDebug>

#include "manifest.hh"

namespace Freebox {
namespace Fileformat {

Manifest::Manifest(const QString &file) :
    m_filename(file)
{
    if (!(m_isValid = read()))
        return;

    m_isValid = check();
}

Manifest::Manifest(const QJsonDocument &jsonDoc)
{
    m_json = jsonDoc;

    if (!m_json.isObject()) {
        m_lastError = QString::fromAscii("JSON document root is not an object");
        return;
    }

    m_obj = m_json.object();

    m_isValid = check();
}

bool Manifest::read()
{
    QFile file(m_filename);

    if (!file.exists())
        return false;

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QByteArray data = file.readAll();

    file.close();

    m_json = QJsonDocument::fromJson(data);

    if (m_json.isNull())
        return false;

    if (!m_json.isObject())
        return false;

    m_obj = m_json.object();

    return true;
}

QJsonValue Manifest::value(const QString &name, const QJsonObject &obj) const
{
    return obj[name];
}

bool Manifest::isStringValue(const QString &name, const QJsonObject &obj) const
{
    return obj[name].isString();
}

QString Manifest::stringValue(const QString &name, const QJsonObject &obj) const
{
    return obj[name].toString();
}

bool Manifest::isNumberValue(const QString &name, const QJsonObject &obj) const
{
    return obj[name].isDouble();
}

int Manifest::intValue(const QString &name, const QJsonObject &obj) const
{
    return obj[name].toInt();
}

QJsonObject Manifest::objectValue(const QString &name, const QJsonObject &obj) const
{
    return obj[name].toObject();
}

bool Manifest::entryPointsIsValid() const
{
    QString epKey = QStringLiteral("entryPoints");

    return m_obj.contains(epKey) && m_obj[epKey].isObject();
}

QStringList Manifest::entryPoints() const
{
    return objectValue(QStringLiteral("entryPoints")).keys();
}

bool Manifest::entryPointsValueIsValid(const QString &entry) const
{
    return value(entry, objectValue(QStringLiteral("entryPoints"))).isObject();
}

bool Manifest::isEntryPointsFileValid(const QString &entry) const
{
    QJsonObject epObj = objectValue(QStringLiteral("entryPoints"));
    QString fileKey = QStringLiteral("file");

    return objectValue(entry, epObj).contains(fileKey)
        && objectValue(entry, epObj)[fileKey].isString();
}

QString Manifest::entryPointsFile(const QString &entry) const
{
    return objectValue(entry, objectValue(QStringLiteral("entryPoints")))[QStringLiteral("file")].toString();
}

QString Manifest::entryPointFile(const QString &entry) const
{
    return entryPointsFile(entry);
}

bool Manifest::isEntryPointsUrlHandlerValid(const QString &entry) const
{
    return objectValue(entry, objectValue(QStringLiteral("entryPoints")))[QStringLiteral("urlHandler")].isArray();
}

QJsonArray Manifest::entryPointsUrlHandler(const QString &entry) const
{
    return objectValue(entry, objectValue(QStringLiteral("entryPoints")))[QStringLiteral("urlHandler")].toArray();
}

QString Manifest::getString(const QString &key) const
{
    if (!m_obj.contains(key) || !isStringValue(key))
        return QString();

    return stringValue(key);
}

QString Manifest::name() const
{
    return getString(QStringLiteral("name"));
}

QString Manifest::identifier() const
{
    return getString(QStringLiteral("identifier"));
}

QString Manifest::description() const
{
    return getString(QStringLiteral("description"));
}

bool Manifest::checkRequiredProperty(QJsonObject &obj, const QString &key, const QString &where)
{
    MAN_ASSERT_ERR(obj.contains(key), return false, QObject::tr("Missing required value for key '%1.%2'").arg(where).arg(key));

    return true;
}

bool Manifest::checkNumberProperty(QJsonObject &obj, const QString &key, int min, int max, const QString &where)
{
    MAN_ASSERT_ERR(isNumberValue(key, obj), return false, QObject::tr("Property '%1.%2' must be a number").arg(where).arg(key));

    int intVal = intValue(key, obj);
    MAN_ASSERT_ERR(intVal >= min && intVal <= max, return false, QObject::tr("Property '%1.%2' must be between %3 and %4 included").arg(where).arg(key).arg(QString::number(min)).arg(QString::number(max)));

    return true;
}

bool Manifest::checkRegExpProperty(QJsonObject &obj, QString &key, QString &where)
{
    MAN_ASSERT_ERR(isStringValue(key, obj), return false, QObject::tr("Property '%1.%2' must be a regexp string").arg(where).arg(key));

    QString val = stringValue(key, obj);

    QRegExp re(val);

    MAN_ASSERT_ERR(re.isValid(), return false, QObject::tr("Invalid regexp in property '%1.%2' (value is '%3')").arg(where).arg(key).arg(val));

    return true;
}

bool Manifest::checkStringProperty(QJsonObject &obj, const QString &key, const QString &regExp, const QString &where)
{
    if (!obj.contains(key))
        return true;

    MAN_ASSERT_ERR(isStringValue(key, obj), return false, QObject::tr("Property '%1.%2' must be a string").arg(where).arg(key));

    QString val = stringValue(key, obj);
    QRegExp re(regExp, Qt::CaseInsensitive);

    MAN_ASSERT_ERR(re.exactMatch(val), return false, QObject::tr("Invalid characters in property '%1.%2' (value is '%3', regexp is /^%4$/i)").arg(where).arg(key).arg(val).arg(regExp));

    return true;
}

bool Manifest::check()
{
    m_nbDefaults = 0;
    m_files.clear();
    m_defaultEntryPoint = QString();

    MAN_ASSERT(checkRequiredProperty(m_obj, QStringLiteral("identifier"), QStringLiteral("(root)")), return false);
    MAN_ASSERT(checkStringProperty(m_obj, QStringLiteral("identifier"), QStringLiteral("\\w+(\\.\\w+)+"), QStringLiteral("(root)")), return false);
    MAN_ASSERT_ERR(entryPointsIsValid(), return false, QObject::tr("Invalid or missing entryPoints"));

    QStringList eps = entryPoints();

    for (quint16 lo = 0; lo < eps.size(); lo++) {
        QString key = eps[lo];

        MAN_ASSERT_ERR(entryPointsValueIsValid(key), return false, QObject::tr("Property 'entryPoints.%1' must be an object").arg(key));
        MAN_ASSERT_ERR(isEntryPointsFileValid(key), return false, QObject::tr("Property 'entryPoints.%1.file' must be an existing file").arg(key));

        QString fileStr = entryPointsFile(key);
        QJsonObject epObj = objectValue(key, objectValue(QStringLiteral("entryPoints")));

        MAN_ASSERT(checkRequiredProperty(epObj, QStringLiteral("file"), QString::fromUtf8("entryPoints.%1").arg(key)), return false);
        MAN_ASSERT(checkStringProperty(epObj, QStringLiteral("file"), QStringLiteral("[/\\w\\.\\+~-]+"), QString::fromUtf8("entryPoints.%1").arg(key)), return false);
        m_files.append(fileStr);

        if (epObj.contains(QStringLiteral("uiFlavor"))) {
            MAN_ASSERT(checkStringProperty(epObj, QStringLiteral("uiFlavor"), QStringLiteral("(multi|classic)"), QString::fromUtf8("entryPoints.%1").arg(key)), return false);
        }

        QString defaultKey = QStringLiteral("default");
        if (epObj.contains(defaultKey)) {
            QJsonValue defaultVal = value(defaultKey, epObj);

            MAN_ASSERT_ERR(defaultVal.isBool(), return false, QObject::tr("Property 'entryPoints.%1.default' must be a bool").arg(key));
            MAN_ASSERT_ERR(!(defaultVal.toBool() && m_nbDefaults++), return false, QObject::tr("Only one property 'entryPoints.<string>.default' can be true"));

            if (defaultVal.toBool())
                m_defaultEntryPoint = key;
        }

        QString urlsKey = QStringLiteral("urlHandler");
        if (epObj.contains(urlsKey)) {
            MAN_ASSERT_ERR(isEntryPointsUrlHandlerValid(key), return false, QObject::tr("Property 'entryPoints.%1.urlHandler' must be an array").arg(key));

            QJsonArray urlsArray = entryPointsUrlHandler(key);

            for (int urlsIndex = 0; urlsIndex < urlsArray.size(); ++urlsIndex) {
                bool seenPattern = false;

                MAN_ASSERT_ERR(urlsArray[urlsIndex].isObject(), return false, QObject::tr("Property 'entryPoints.%1.urlHandler[%2]' must be an object").arg(key).arg(QString::number(urlsIndex)));

                QJsonObject urlsArrayVal = urlsArray[urlsIndex].toObject();
                QString where = QString::fromUtf8("entryPoints.%1.urlHandler[%2]").arg(key).arg(urlsIndex);

                QString urlPatKey = QStringLiteral("urlPattern");
                if (urlsArrayVal.contains(urlPatKey)) {
                    seenPattern = true;
                    MAN_ASSERT(checkRegExpProperty(urlsArrayVal, urlPatKey, where), return false);
                }

                QString mimePatKey = QStringLiteral("mimeTypePattern");
                if (urlsArrayVal.contains(mimePatKey)) {
                    seenPattern = true;
                    MAN_ASSERT(checkRegExpProperty(urlsArrayVal, mimePatKey, where), return false);
                }

                MAN_ASSERT_ERR(seenPattern, return false, QObject::tr("Property 'entryPoints.%1.urlHandler[%2]' must contain one of 'urlPattern' or 'mimeTypePattern' regexps, or both.").arg(key).arg(QString::number(urlsIndex)));

                MAN_ASSERT(checkRequiredProperty(urlsArrayVal, QStringLiteral("actionId"), where), return false);
                MAN_ASSERT(checkStringProperty(urlsArrayVal, QStringLiteral("actionId"), QStringLiteral("\\w{1,40}"), where), return false);

                MAN_ASSERT(checkRequiredProperty(urlsArrayVal, QStringLiteral("actionLabel"), where), return false);
                MAN_ASSERT(checkStringProperty(urlsArrayVal, QStringLiteral("actionLabel"), QStringLiteral("[\\w '\"\\.\\+\\-\\!\\?:]{1,40}"), where), return false);

                MAN_ASSERT(checkRequiredProperty(urlsArrayVal, QStringLiteral("score"), where), return false);
                MAN_ASSERT(checkNumberProperty(urlsArrayVal, QStringLiteral("score"), 0, 1000, where), return false);
            }
        }
    }

    return true;
}

} // namespace Fileformat
} // namespace Freebox
