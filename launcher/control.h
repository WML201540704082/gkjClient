#ifndef CONTROL_H
#define CONTROL_H

#include "screenstatuscheck.h"
#include "shutdownmonitor.h"
#include "databasemanager.h"

#include <QObject>
#include <QThread>

class control : public QObject
{
    Q_OBJECT
public:
    explicit control(QObject *parent = nullptr);
    
public slots:
    void onLogout();
    
signals:
    void checkStart();
    
private:
    databaseManager dbManager;
    
    screenStatusCheck *screenStatusCheckWork;
    QThread *checkThread;
    ShutdownMonitor *shutdownMonitor; // 新增：关机监听器（主线程）
};

#endif // CONTROL_H
