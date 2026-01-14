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
    
}

sendNetworkInfoWork::~sendNetworkInfoWork()
{
    if(client)
    {
        delete client;
        client = nullptr;
    }
}

void sendNetworkInfoWork::recheckTimestamps()
{
    static unsigned short count =0;
    qDebug()<<"count: "<<count;
    if(count > 1)
    {
        qDebug()<<"重新对时次数过多，终止尝试";
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
    
    networkMonitor monitor;
    QList<networkInfo> networkInfoList;
    
    databaseManager2 dbManager2;
    dbManager2.connectToDatabase();
    
    QString username;
    QString department;
    dbManager2.queryLatestLoginRecord(username, department);
    
    
    // 获取TCP连接信息
    auto tcpConns = monitor.getTcpConnections();
    for (const auto& conn : tcpConns)
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
    
    if(networkInfoList.isEmpty())
    {
        qDebug()<<"采集到的网络数据为空";
        return;
    }
    
    //拼接json
    QJsonArray networkInfoArray;
    for(auto networkInfo : networkInfoList)
    {
        QJsonObject networkInfoObj;
        
        networkInfoObj["protocol"] = networkInfo.protocol;
        networkInfoObj["localAddr"] = networkInfo.localAddr;
        networkInfoObj["localPort"] = networkInfo.localPort;
        networkInfoObj["remoteAddr"] = networkInfo.remoteAddr;
        networkInfoObj["remotePort"] = networkInfo.remotePort;
        networkInfoObj["status"] = networkInfo.status;
        
        networkInfoArray.append(networkInfoObj);
    }
    
    QJsonObject data;
    data["networkDataList"] = networkInfoArray;
    data["username"] = username;
    data["department"] = department;
    
    //加密
    QJsonDocument doc = QJsonDocument(data);
    QByteArray docBytes = doc.toJson();
     
    mySm4 sm4;
    unsigned char *docData = reinterpret_cast<unsigned char*>(docBytes.data());
    unsigned long docLen = docBytes.size();  // 不包含结尾的 '\0'
    QString encryptedStr;
    bool res = sm4.encryptStr(docData, docLen, encryptedStr);
    if(!res)
    {
        qDebug()<<"Failed to encrypt json";
        return;
    }
    
    //加密成功继续拼接json
    QJsonObject root;
    root["method"] = "uploadPowerOnOffTime";
    root["data"] = encryptedStr;
    
    //初始化client并发送
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
        }
        else if(obj.value("code").toInt() == 10005) //发送的时间戳与服务端时间差超过预设值，重新对时
        {
            qDebug()<<"response error code: "<<obj.value("code").toInt();
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
    qDebug()<<"send error: "<<errorString;
}

void sendNetworkInfoWork::handleFail()
{
    qDebug()<<"重传次数过多";
}

void sendNetworkInfoWork::onCalculateSuccess()
{
    startSending();
}
