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
#ifndef MANIFEST_HH_
# define MANIFEST_HH_

# include <QFile>
# include <QJsonDocument>
# include <QJsonValue>

namespace Freebox {
namespace Fileformat {

class Manifest
{
public:
    Manifest();
    Manifest(const QString &file);

    QString name() const;
    QString identifier() const;
    QString description() const;
    QStringList entryPoints() const;
    QString entryPointFile(const QString &entry) const;

    bool isValid() const;

private:
    void read();
    QJsonValue value(const QString &name) const;
    QString stringValue(const QString &name) const;

    QString m_filename;
    QJsonDocument m_json;
};

} // namespace Fileformat
} // namespace Freebox

#endif // !MANIFEST_HH_
