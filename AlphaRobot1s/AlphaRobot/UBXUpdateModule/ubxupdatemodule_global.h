#ifndef UBXUPDATEMODULE_GLOBAL_H
#define UBXUPDATEMODULE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(UBXUPDATEMODULE_LIBRARY)
#  define UBXUPDATEMODULESHARED_EXPORT Q_DECL_EXPORT
#else
#  define UBXUPDATEMODULESHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // UBXUPDATEMODULE_GLOBAL_H
