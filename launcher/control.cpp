#include "control.h"
#include "utf8.h"

#include <QDebug>
#include <QCoreApplication>

control::control(QObject *parent) : QObject(parent)
{
    qDebug() << "[control] 构造函数，主线程ID:" << QThread::currentThreadId();
    
    dbManager.connectToDatabase();
    dbManager.createTables();
    
    //初始化监听锁屏线程
    screenStatusCheckWork = new screenStatusCheck();
    checkThread = new QThread();
    
    screenStatusCheckWork->moveToThread(checkThread);
    
    connect(checkThread, &QThread::finished, screenStatusCheckWork, &screenStatusCheck::deleteLater);
    connect(this, &control::checkStart, screenStatusCheckWork, &screenStatusCheck::onCheckStart);
    connect(screenStatusCheckWork, &screenStatusCheck::logout, this, &control::onLogout);
    
    checkThread->start();
    
    //创建主线程的关机监听器（必须在checkThread启动后）
    shutdownMonitor = new ShutdownMonitor(this);
    
    // 方式1：连接到子线程（跨线程，Qt自动排队）
    connect(shutdownMonitor, &ShutdownMonitor::systemShuttingDown, 
            screenStatusCheckWork, &screenStatusCheck::onSystemShutdown,
            Qt::QueuedConnection);
    
    // 方式2：直接在主线程执行（更可靠）
    connect(shutdownMonitor, &ShutdownMonitor::systemShuttingDown, 
            this, &control::onLogout, 
            Qt::DirectConnection); // 直接连接，立即执行
    
    qDebug() << "[control] 监控系统初始化完成";
    
    emit checkStart();
}

//void control::onLogout()
//{
//    // 同步更新数据库
//    bool res = dbManager.updateLastLogoutTime();
    
//    if(!res)
//    {
//        qDebug() << "更新登出时间失败";
//    }
//    else
//    {
//        qDebug() << "更新登出时间成功";
//    }
    
//    // 确保事件处理完成
//    QCoreApplication::processEvents();
//}


void control::onLogout()
{
    qDebug() << "========================================";
    qDebug() << "[control] onLogout() 被调用";
    qDebug() << "[control] 线程ID:" << QThread::currentThreadId();
    qDebug() << "========================================";
    
    // 同步更新数据库
    bool res = dbManager.updateLastLogoutTime();
    
    if(!res)
    {
        qDebug() << "[control] ××× 更新登出时间失败 ×××";
    }
    else
    {
        qDebug() << "[control] √√√ 更新登出时间成功 √√√";
    }
    
    // 确保事件处理完成
    QCoreApplication::processEvents();
    
    // 增加等待时间确保数据库写入
    QThread::msleep(200);
    
    qDebug() << "[control] onLogout() 执行完成";
    qDebug() << "========================================";
}
