#include "control.h"
#include "model/boottime.h"
#include "utf8.h"

#include <QDateTime>
#include <QDebug>
#include <QThread>
#include <QCoreApplication>

control::control(QObject *parent) : QObject(parent)
{
    dbManager.connectToDatabase();
    dbManager.createTables();
    
    bootTime timeObj;
    QDateTime bootTime = timeObj.getBootTimeViaWMI();
    
    if (bootTime.isValid())
    {
        qDebug() << "系统开机时间:" << bootTime.toString("yyyy-MM-dd hh:mm:ss");
        
        QString latestPowerOnTime;
        dbManager.queryLatestPowerOnTime(latestPowerOnTime);
        if(latestPowerOnTime != bootTime.toString("yyyy-MM-dd hh:mm:ss"))
        {
            dbManager.insertPowerOnTime(bootTime.toString("yyyy-MM-dd hh:mm:ss"));
        }

        qint64 uptime = bootTime.secsTo(QDateTime::currentDateTime());
        qDebug() << "系统已运行:" << uptime / 3600 << "小时" << (uptime % 3600) / 60 << "分钟";
    } 
    else 
    {
        qDebug() << "获取开机时间失败";
    }
    
    //创建主线程的关机监听器（必须在checkThread启动后）
    shutdownMonitor = new ShutdownMonitor(this);
    
    connect(shutdownMonitor, &ShutdownMonitor::systemShuttingDown, 
            this, &control::onSystemShuttingDown, 
            Qt::DirectConnection); // 直接连接，立即执行
    
    iniSyncThread();
}

void control::iniSyncThread()
{
    pSyncWork = new syncWork();
    syncThread = new QThread();
    
    pSyncWork->moveToThread(syncThread);
    
    connect(syncThread, &QThread::finished, pSyncWork, &syncWork::deleteLater);
    connect(this, &control::iniSyncWork, pSyncWork, &syncWork::onIniSyncWork);
    connect(this, &control::startSyncWork, pSyncWork, &syncWork::onStartSyncWork);
    
//    qDebug() << "main thread id: " << QThread::currentThreadId();
    
    syncThread->start();
    
    emit iniSyncWork();
    emit startSyncWork();
}

void control::onSystemShuttingDown()
{
    qDebug() << "========================================";
    qDebug() << "[control] onSystemShuttingDown() 被调用";
    qDebug() << "[control] 线程ID:" << QThread::currentThreadId();
    qDebug() << "========================================";
    
    // 同步更新数据库
    bool res = dbManager.updatePowerOffTime();
    
    if(!res)
    {
        qDebug() << "[control] ××× 更新关机时间失败 ×××";
    }
    else
    {
        qDebug() << "[control] √√√ 更新关机时间成功 √√√";
    }
    
    // 确保事件处理完成
    QCoreApplication::processEvents();
    
    // 增加等待时间确保数据库写入
    QThread::msleep(200);
    
    qDebug() << "[control] onSystemShuttingDown() 执行完成";
    qDebug() << "========================================";
}
