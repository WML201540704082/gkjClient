#ifndef CAMINIWORK_H
#define CAMINIWORK_H

#include "globalVariables.h"
#include "dvtcamera.h"

#include <QObject>

class camIniWork : public QObject
{
    Q_OBJECT
public:
    explicit camIniWork(QObject *parent = nullptr);
    
public slots:
    void onStartIniCam(DvtCamera *pCamera);
    
    
signals:
    
};

#endif // CAMINIWORK_H
