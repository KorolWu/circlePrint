#ifndef MCCLIENT_GLOBAL_H
#define MCCLIENT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MCCLIENT_LIBRARY)
#  define MCCLIENT_EXPORT Q_DECL_EXPORT
#else
#  define MCCLIENT_EXPORT Q_DECL_IMPORT
#endif

#endif // MCCLIENT_GLOBAL_H
