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
#include <QDebug>
#include <QRegExp>

#include "http.hh"

namespace Freebox {
namespace Http {

Method::Method(const QString &string)
{
    if (string == QStringLiteral("GET"))
        mName = GET;
    else
        mName = UNKNOWN;
}

Method::operator QString() const
{
    switch (mName) {
    case GET:
        return QStringLiteral("GET");
    default:
        break;
    }
    return QStringLiteral("UNKNOWN");
}

bool Method::operator==(const enum name n)
{
    return (mName == n);
}

Version::Version() :
    mValid(false)
{
}

Version::Version(unsigned char major, unsigned char minor) :
    mValid(true), mMajor(major), mMinor(minor)
{
}

Version::Version(const QString &major, const QString &minor) :
    mValid(true),
    mMajor(major.toUInt()),
    mMinor(minor.toUInt())
{
}

Version::Version(const QString &str) :
    mValid(false)
{
    QRegExp ex(QStringLiteral("HTTP/(\\d)\\.(\\d)"));

    if (ex.exactMatch(str)) {
        mValid = true;
        mMajor = ex.cap(1).toUInt();
        mMinor = ex.cap(2).toUInt();
    }
}

Version::operator QString () const
{
    return QStringLiteral("HTTP/%1.%2")
        .arg(QString::number(mMajor))
        .arg(QString::number(mMinor));
}

bool Version::isValid() const
{
    return mValid;
}

unsigned char Version::major() const
{
    return mMajor;
}

unsigned char Version::minor() const
{
    return mMinor;
}

} // namespace Http
} // namespace Freebox
