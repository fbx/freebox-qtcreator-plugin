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
#ifndef SSDPMESSAGE_HH_
# define SSDPMESSAGE_HH_

# include <QString>
# include <QRegExp>
# include <QHash>

namespace Freebox {
namespace Ssdp {

class Message
{
public:
    enum type {
        INVALID,
        MSEARCH,
        NOTIFY,
        REPLY,
    };

    Message(const char *data);

    bool isValid() const { return mType != INVALID; }
    enum type type() const { return mType; }
    QString name() const;
    QStringList fields() const;
    QString fieldValue(QString name) const;

private:
    const QRegExp msearch;
    const QRegExp notify;
    const QRegExp reply;
    const QRegExp field;
    enum type mType;
    QString mData;
    QString mTypeString;
    QHash<QString, QString> mHeaders;

};

} // namespace Ssdp
} // namespace Freebox

#endif // !SSDPMESSAGE_HH_
