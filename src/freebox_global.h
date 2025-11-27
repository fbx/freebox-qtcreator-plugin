#pragma once

#include <qglobal.h>

#if defined(FREEBOX_LIBRARY)
#  define FREEBOXSHARED_EXPORT Q_DECL_EXPORT
#elif defined(FREEBOX_STATIC_LIBRARY)
#  define FREEBOXSHARED_EXPORT
#else
#  define FREEBOXSHARED_EXPORT Q_DECL_IMPORT
#endif
