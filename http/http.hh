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
#ifndef HTTPD_H
#define HTTPD_H

#include <QString>
#include <QTextStream>
#include "constants.hh"

namespace Freebox {
namespace Http {

static const char CRLF[] = "\r\n";
static const char CRLFCRLF[] = "\r\n\r\n";

class IResponse
{
public:
    virtual unsigned int code() const = 0;
    virtual const char *message() const = 0;
};

template<unsigned int Code, const char *Name>
class Response : public IResponse
{
public:
    unsigned int code() const { return Code; }
    const char *message() const { return Name; }
};

namespace Reply {
typedef Response<200, Constants::OK>                    Ok;
typedef Response<400, Constants::BAD_REQUEST>           BadRequest;
typedef Response<404, Constants::NOT_FOUND>             NotFound;
typedef Response<500, Constants::INTERNAL_SERVER_ERROR> InternalServerError;
} // namespace Reply

class Method
{
public:

    enum name {
        GET,
        UNKNOWN,
    };

    Method(const QString &string);
    operator QString() const;
    bool operator ==(const enum name n);

private:
    enum name mName;
};

class Version
{
public:
    Version();
    Version(unsigned char major, unsigned char minor);
    Version(const QString &major, const QString &minor);
    Version(const QString &);

    operator QString() const;

    bool isValid() const;
    unsigned char major() const;
    unsigned char minor() const;

private:
    bool mValid;
    unsigned char mMajor;
    unsigned char mMinor;
};

} // namespace Http
} // namespace Freebox

#endif // HTTPD_H
