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
#ifndef CONSTANTS_HH_
# define CONSTANTS_HH_

#ifdef __GNUC__

namespace Freebox {
namespace Http {
namespace Constants {

constexpr const char OK[] = "OK";
constexpr const char BAD_REQUEST[] = "Bad request";
constexpr const char NOT_FOUND[] = "Not found";
constexpr const char INTERNAL_SERVER_ERROR[] = "Internal server error";

} // namespace Constants
} // namespace Http
} // namespace Freebox

#else // workaround msvc2010 not being c++11 compliant

#define HTTP_CONSTANT_OK "OK"
#define HTTP_CONSTANT_BAD_REQUEST  "Bad request";
#define HTTP_CONSTANT_NOT_FOUND "Not found";
#define HTTP_CONSTANT_INTERNAL_SERVER_ERROR "Internal server error";

#endif // __GNUC__

#endif // !CONSTANTS_HH_
