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
#include <QStringList>

#include "ssdpmessage.hh"

namespace Freebox {
namespace Ssdp {

Message::Message(const char *data) :
    msearch(QRegExp(QStringLiteral("M-SEARCH\\s+\\*\\s+HTTP/1\\.1"))),
    notify(QRegExp(QStringLiteral("NOTIFY\\s+\\*\\s+HTTP/1\\.1"))),
    reply(QRegExp(QStringLiteral("HTTP/1\\.1 200 OK"))),
    field(QRegExp(QStringLiteral("([^:]*):(.*)"))),
    mType(INVALID),
    mData(QString::fromUtf8(data))
{

    QStringList fields = mData.split(QStringLiteral("\r\n"), QString::SkipEmptyParts);

    if (fields.size() < 1)
        return;

    mTypeString = fields.takeFirst();

    if (msearch.exactMatch(mTypeString))
        mType = MSEARCH;
    else if (notify.exactMatch(mTypeString))
        mType = NOTIFY;
    else if (reply.exactMatch(mTypeString))
        mType = REPLY;
    else
        qWarning("Unknown SSDP message %s", mTypeString.toLocal8Bit().constData());

    foreach(QString f, fields) {
        if (field.indexIn(f) < 0) {
            qWarning("Invalid field %s", f.toLocal8Bit().constData());
        }
        else {
            mHeaders[field.cap(1).simplified()] = field.cap(2).simplified();
        }
    }
}

QString Message::name() const
{
    switch (mType) {
    case MSEARCH:   return QStringLiteral("msearch");
    case NOTIFY:    return QStringLiteral("notify");
    case REPLY:     return QStringLiteral("reply");
    case INVALID:
    default:
        break;
    }

    return QString();
}

QStringList Message::fields() const
{
    return mHeaders.keys();
}

QString Message::fieldValue(QString name) const
{
    return mHeaders[name];
}

} // namespace Ssdp
} // namespace Freebox
