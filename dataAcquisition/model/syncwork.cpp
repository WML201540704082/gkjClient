#include "syncwork.h"
#include "globalVariables.h"
#include "model/databasemanager.h"
#include "model/myStruct.h"

#include <QThread>

syncWork::syncWork(QObject *parent) : QObject(parent)
{
    
}

syncWork::~syncWork()
{
    if(getTimestampsThread)
    {
        getTimestampsThread->quit();
        getTimestampsThread->wait();
        delete getTimestampsThread;
    }
    
    if(timer)
    {
        delete timer;
        timer = nullptr;
    }
    
    if(timer2)
    {
        delete timer2;
        timer2 = nullptr;
    }
    
    if(timer3)
    {
        delete timer3;
        timer3 = nullptr;
    }
    
    if(sendLocalAppInfoThread)
    {
        sendLocalAppInfoThread->quit();
        sendLocalAppInfoThread->wait();
        delete sendLocalAppInfoThread;
    }
}

void syncWork::sendSyncData()
{
    //先获取需要同步的数据
    databaseManager dbManager;
    dbManager.connectToDatabase();
    QList<QList<QString>> timeList;
    bool res = dbManager.queryUnsyncTime(timeList);
    qDebug()<<timeList.size();
    qDebug()<<timeList;
    
    QList<powerOnOffTimeInfo> powerOnOffTimeList;
    for(const auto& time : timeList)
    {
        powerOnOffTimeInfo tmpTimeInfo;
        tmpTimeInfo.powerOnTime = time[0];
        tmpTimeInfo.powerOffTime = time[1];
        
        powerOnOffTimeList.append(tmpTimeInfo);
    }
    
    if(!sendPowerOnOffThread->isRunning())
    {
        sendPowerOnOffThread->start();
    }
    emit startSendTimeInfo(powerOnOffTimeList);
    
    if(!sendNetworkInfoThread->isRunning())
    {
        sendNetworkInfoThread->start();
    }
    timer2 = new QTimer(this);
    connect(timer2, &QTimer::timeout, this, [this]()
    {
        emit startSendNetworkInfo();
    });
    emit startSendNetworkInfo();
    timer2->start(30000);
    
    if(!sendLocalAppInfoThread->isRunning())
    {
        sendLocalAppInfoThread->start();
    }
    timer3 = new QTimer(this);
    connect(timer3, &QTimer::timeout, this, [this]()
    {
        emit startSendLocalAppInfo();
    });
    emit startSendLocalAppInfo();
    timer3->start(30000);
}

void syncWork::onIniSyncWork()
{
    qDebug() << "sync thread id: " << QThread::currentThreadId();
    timestampsWork = new getTimestampsWork();
    getTimestampsThread = new QThread();
    timestampsWork->moveToThread(getTimestampsThread);
//    connect(getTimestampsThread, &QThread::finished, timestampsWork, &QObject::deleteLater);
    connect(this, &syncWork::startGetTimestamps, timestampsWork, &getTimestampsWork::startSending);
    connect(timestampsWork, &getTimestampsWork::calculateSuccess, this, &syncWork::receiveCalculateSuccess);
//    connect(timestampsWork, &getTimestampsWork::workFinished, getTimestampsThread, &QThread::quit);    

    powerOnOffWork = new sendPowerOnOffWork();
    sendPowerOnOffThread = new QThread();
    powerOnOffWork->moveToThread(sendPowerOnOffThread);
    connect(sendPowerOnOffThread, &QThread::finished, powerOnOffWork, &QObject::deleteLater);
    qRegisterMetaType<QList<powerOnOffTimeInfo>>("QList<powerOnOffTimeInfo>");
    connect(this, &syncWork::startSendTimeInfo, powerOnOffWork, &sendPowerOnOffWork::startSending);    
    connect(powerOnOffWork, &sendPowerOnOffWork::sendTimeInfoSuccessfully, this, &syncWork::onSendTimeInfoSuccessfully);       

    networkInfoWork = new sendNetworkInfoWork();
    sendNetworkInfoThread = new QThread();
    networkInfoWork->moveToThread(sendNetworkInfoThread);
    connect(sendNetworkInfoThread, &QThread::finished, networkInfoWork, &QObject::deleteLater);
    connect(this, &syncWork::startSendNetworkInfo, networkInfoWork, &sendNetworkInfoWork::startSending);        

    localAppInfoWork = new sendLocalAppInfoWork();
    sendLocalAppInfoThread = new QThread();
    localAppInfoWork->moveToThread(sendLocalAppInfoThread);
    connect(sendLocalAppInfoThread, &QThread::finished, localAppInfoWork, &QObject::deleteLater);
    connect(this, &syncWork::startSendLocalAppInfo, localAppInfoWork, &sendLocalAppInfoWork::startSending);
}

void syncWork::onStartSyncWork()
{
    if(!getTimestampsThread->isRunning())
    {
        getTimestampsThread->start();
    }
    
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]()
    {
        emit startGetTimestamps(urlCommon + "/time");
    });
    emit startGetTimestamps(urlCommon + "/time");
    timer->start(300000); // 对时成功之前，每5分钟尝试一次对时，一次判断网络是否连通
}

void syncWork::receiveCalculateSuccess()
{
    if(timer)
    {
        delete timer;
        timer = nullptr;
    }
    sendSyncData();
}

void syncWork::onSendTimeInfoSuccessfully(QList<powerOnOffTimeInfo> timeList)
{
    //收到同步成功信号，将相应数据的状态修改为已同步
    databaseManager dbManager;
    dbManager.connectToDatabase();
    
    for(auto timeInfo : timeList)
    {  
        QString powerOnTime = timeInfo.powerOnTime;
        QString powerOffTime = timeInfo.powerOffTime;
        
        bool res = dbManager.updateTimeSyncRecord(powerOnTime, powerOffTime);
        if(!res)
        {
            qDebug()<<"update LoginLog failed: "<<powerOnTime <<" "<<powerOffTime;
        } 
    }
}
