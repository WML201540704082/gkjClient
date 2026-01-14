#include "caminiwork.h"

camIniWork::camIniWork(QObject *parent) : QObject(parent)
{
    
}

void camIniWork::onStartIniCam(DvtCamera *pCamera)
{
    pCamera->iniCamera();
}

