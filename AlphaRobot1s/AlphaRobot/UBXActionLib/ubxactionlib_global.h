#ifndef UBXACTIONLIB_GLOBAL_H
#define UBXACTIONLIB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(UBXACTIONLIB_LIBRARY)
#  define UBXACTIONLIBSHARED_EXPORT Q_DECL_EXPORT
#else
#  define UBXACTIONLIBSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // UBXACTIONLIB_GLOBAL_H