#ifndef DVTCAMERA_GLOBAL_H
#define DVTCAMERA_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(DVTCAMERA_LIBRARY)
#  define DVTCAMERA_EXPORT Q_DECL_EXPORT
#else
#  define DVTCAMERA_EXPORT Q_DECL_IMPORT
#endif

#endif // DVTCAMERA_GLOBAL_H
