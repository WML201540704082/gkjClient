#ifndef CONTROL_H
#define CONTROL_H

#include "model/databasemanager.h"
#include "model/shutdownmonitor.h"
#include "model/syncwork.h"
#include "model/localappmonitor.h"

#include <QObject>

class control : public QObject
{
    Q_OBJECT
public:
    explicit control(QObject *parent = nullptr);
    
    void iniSyncThread();
    
public slots:
    void onSystemShuttingDown();
    
signals:
    void startGetTimestamps();
    void iniSyncWork();
    void startSyncWork();
    
private:
    databaseManager dbManager;
    
    ShutdownMonitor *shutdownMonitor; // 新增：关机监听器（主线程）
    LocalAppMonitor *localAppMonitor; // 新增：本地桌面应用监控器
    
    syncWork *pSyncWork = nullptr;
    QThread *syncThread = nullptr;
};

#endif // CONTROL_H
