#ifndef GLOBALVARIABLES_H
#define GLOBALVARIABLES_H

#include "windows.h"

#include <QString>

extern double w_fx; // 横向缩放因子
extern double h_fx; // 纵向缩放因子
extern QString fingerprint; //指纹信息
extern volatile qint64 timeDifference; //时间差
extern QString urlCommon;
extern HANDLE m_Handle;
extern float similarityThreshold;//人脸相似度阈值

#endif // GLOBALVARIABLES_H
