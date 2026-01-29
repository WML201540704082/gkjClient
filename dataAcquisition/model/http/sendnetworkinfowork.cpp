#include "sendnetworkinfowork.h"
#include "model/http/gettimestampswork.h"
#include "model/networkmonitor.h"
#include "model/myStruct.h"
#include "model/mysm4.h"
#include "globalVariables.h"
#include "model/databasemanager2.h"

#include <QThread>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

sendNetworkInfoWork::sendNetworkInfoWork(QObject *parent) : QObject(parent)
{
    monitor = new networkMonitor();
}

sendNetworkInfoWork::~sendNetworkInfoWork()
{
    if(client)
    {
        delete client;
        client = nullptr;
    }
    
    if(monitor)
    {
        delete monitor;
        monitor = nullptr;
    }
}

void sendNetworkInfoWork::recheckTimestamps()
{
    static unsigned short count =0;
    qDebug()<<"count: "<<count;
    if(count > 1)
    {
        qDebug()<<"Too many recheck attempts, terminating";
        return;
    }
    
    getTimestampsWork* tmpTimestampsWork = new getTimestampsWork();
    QThread* tmpGetTimestampsThread = new QThread();
    tmpTimestampsWork->moveToThread(tmpGetTimestampsThread);
    connect(tmpGetTimestampsThread, &QThread::finished, tmpTimestampsWork, &QObject::deleteLater);
    connect(this, &sendNetworkInfoWork::startGetTimestamps, tmpTimestampsWork, &getTimestampsWork::startSending);
    connect(tmpTimestampsWork, &getTimestampsWork::calculateSuccess, this, &sendNetworkInfoWork::onCalculateSuccess);
    connect(tmpTimestampsWork, &getTimestampsWork::workFinished, tmpGetTimestampsThread, &QThread::quit);
    
    tmpGetTimestampsThread->start();
    emit startGetTimestamps(urlCommon + "/time");
    
    count++;
}

void sendNetworkInfoWork::startSending()
{
    if(client)
    {
        delete client;
        client = nullptr;
    }
    
    QList<networkInfo> networkInfoList;
    
    databaseManager2 dbManager2;
    dbManager2.connectToDatabase();
    
    QString username;
    QString department;
    dbManager2.queryLatestLoginRecord(username, department);
    
    QList<ConnectionInfo> tcpConns = monitor->getTcpConnections();
    for (int i = 0; i < tcpConns.size(); i++)
    {
        const ConnectionInfo& conn = tcpConns[i];
        networkInfo tmpInfo;
        tmpInfo.protocol = conn.protocol;
        tmpInfo.localAddr = conn.localAddress;
        tmpInfo.localPort = conn.localPort;
        tmpInfo.remoteAddr = conn.remoteAddress;
        tmpInfo.remotePort = conn.remotePort;
        tmpInfo.status = conn.state;
        
        networkInfoList.append(tmpInfo);
    }
    
    QList<ConnectionInfo> udpConns = monitor->getUdpConnections();
    for (int i = 0; i < udpConns.size(); i++)
    {
        const ConnectionInfo& conn = udpConns[i];
        if (conn.remoteAddress != "*")
        {
            networkInfo tmpInfo;
            tmpInfo.protocol = conn.protocol;
            tmpInfo.localAddr = conn.localAddress;
            tmpInfo.localPort = conn.localPort;
            tmpInfo.remoteAddr = conn.remoteAddress;
            tmpInfo.remotePort = conn.remotePort;
            tmpInfo.status = conn.state;
            
            networkInfoList.append(tmpInfo);
        }
    }
    
    // 更新IP访问记录
    monitor->updateIPAccessRecords();
    
    // 获取待发送的已结束连接记录
    QList<IPAccessRecord> pendingRecords = monitor->getPendingSendRecords();
    
    if (pendingRecords.isEmpty())
    {
        qDebug() << "No ended connections to send, skipping...";
        return;
    }
    
    // 创建IP到访问记录的映射，方便快速查找
    QMap<QString, IPAccessRecord> pendingRecordMap;
    for (int i = 0; i < pendingRecords.size(); i++)
    {
        const IPAccessRecord& record = pendingRecords[i];
        pendingRecordMap.insert(record.ip, record);
    }
    
    QJsonArray networkInfoArray;
    // 直接基于待发送队列中的记录生成发送数据
    for (int i = 0; i < pendingRecords.size(); i++)
    {
        const IPAccessRecord& record = pendingRecords[i];
        QJsonObject networkInfoObj;
        
        // 添加基本字段
        networkInfoObj["protocol"] = "TCP";
        networkInfoObj["localAddr"] = "";
        networkInfoObj["localPort"] = 0;
        networkInfoObj["remoteAddr"] = record.ip;
        networkInfoObj["remotePort"] = 0;
        networkInfoObj["status"] = "CLOSED";
        
        // 添加username和department字段
        networkInfoObj["username"] = username;
        networkInfoObj["department"] = department;
        
        // 添加IP访问记录的时间信息
        networkInfoObj["startTime"] = record.startTime.toString("yyyy-MM-dd hh:mm:ss");
        networkInfoObj["endTime"] = record.endTime.toString("yyyy-MM-dd hh:mm:ss");
        
        networkInfoArray.append(networkInfoObj);
        
        qDebug() << "Adding ended connection for IP:" << record.ip 
                 << "Start:" << record.startTime.toString("yyyy-MM-dd hh:mm:ss") 
                 << "End:" << record.endTime.toString("yyyy-MM-dd hh:mm:ss");
    }
    
    if (networkInfoArray.isEmpty())
    {
        qDebug() << "No network data for ended connections, skipping...";
        return;
    }
    
    QJsonObject data;
    data["networkDataList"] = networkInfoArray;
    
    QJsonDocument doc = QJsonDocument(data);
    QByteArray docBytes = doc.toJson();
     
    mySm4 sm4;
    unsigned char *docData = reinterpret_cast<unsigned char*>(docBytes.data());
    unsigned long docLen = docBytes.size();
    QString encryptedStr;
    bool res = sm4.encryptStr(docData, docLen, encryptedStr);
    if(!res)
    {
        return;
    }
    
    QJsonObject root;
    root["method"] = "uploadNetworkData";
    root["data"] = encryptedStr;
    
    client = new httpClient();
    connect(client, &httpClient::requestSuccess, this, &sendNetworkInfoWork::handleSuccess);
    connect(client, &httpClient::requestError, this, &sendNetworkInfoWork::handleError);
    connect(client, &httpClient::sendFail, this, &sendNetworkInfoWork::handleFail);
    
    QUrl url = QUrl(urlCommon + "/uploadNetworkData");
    client->post(url, QJsonDocument(root).toJson(QJsonDocument::Compact));
    
}

void sendNetworkInfoWork::handleSuccess(const QByteArray &response)
{
    QJsonDocument doc = QJsonDocument::fromJson(response);
    qDebug()<<doc.isNull();
    if(!doc.isNull())
    {
        QJsonObject obj = doc.object();
        if(obj.value("code").toInt() == 200)
        {
            qDebug()<<"response success code: "<<obj.value("code").toInt();
            // 发送成功，清除已发送的记录
            monitor->clearPendingSendRecords();
        }
        else if(obj.value("code").toInt() == 10005)
        {
            recheckTimestamps();
        }
        else
        {
            qDebug()<<"response error code: "<<obj.value("code").toInt();
        }
    } 
}

void sendNetworkInfoWork::handleError(const QString &errorString)
{
}

void sendNetworkInfoWork::handleFail()
{
}

void sendNetworkInfoWork::onCalculateSuccess()
{
    startSending();
}
