#include "sendpoweronoffwork.h"
#include "model/http/gettimestampswork.h"
#include "globalVariables.h"
#include "model/mysm4.h"
#include "utf8.h"

#include <QThread>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

sendPowerOnOffWork::sendPowerOnOffWork(QObject *parent) : QObject(parent)
{
    
}

sendPowerOnOffWork::~sendPowerOnOffWork()
{
    if(client)
    {
        delete client;
        client = nullptr;
    }
}

void sendPowerOnOffWork::recheckTimestamps()
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
    connect(this, &sendPowerOnOffWork::startGetTimestamps, tmpTimestampsWork, &getTimestampsWork::startSending);
    connect(tmpTimestampsWork, &getTimestampsWork::calculateSuccess, this, &sendPowerOnOffWork::onCalculateSuccess);
    connect(tmpTimestampsWork, &getTimestampsWork::workFinished, tmpGetTimestampsThread, &QThread::quit);
    
    tmpGetTimestampsThread->start();
    emit startGetTimestamps(urlCommon + "/time");
    
    count++;
}

void sendPowerOnOffWork::startSending(const QList<powerOnOffTimeInfo> &timeList)
{
    qDebug() << "sendPowerOnOffWork thread id: " << QThread::currentThreadId();
    curTimeList = timeList;
    if(curTimeList.isEmpty())
    {
        return;
    }
    if(client)
    {
        delete client;
        client = nullptr;
    }
    
    //拼接json
    QJsonArray timeInfoArray;
    for(auto timeInfo : timeList)
    {
        QJsonObject timeInfoObj;
        
        timeInfoObj["powerOnTime"] = timeInfo.powerOnTime;
        timeInfoObj["powerOffTime"] = timeInfo.powerOffTime;
        
        timeInfoArray.append(timeInfoObj);
    }
    
    QJsonObject data;
    data["timeList"] = timeInfoArray;
    
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
    root["method"] = "uploadTimeList";
    root["data"] = encryptedStr;
    
    //初始化client并发送
    client = new httpClient();
    connect(client, &httpClient::requestSuccess, this, &sendPowerOnOffWork::handleSuccess);
    connect(client, &httpClient::requestError, this, &sendPowerOnOffWork::handleError);
    connect(client, &httpClient::sendFail, this, &sendPowerOnOffWork::handleFail);
    
    QUrl url = QUrl(urlCommon + "/uploadTimeList");
    client->post(url, QJsonDocument(root).toJson(QJsonDocument::Compact));
}

void sendPowerOnOffWork::handleSuccess(const QByteArray &response)
{
    QJsonDocument doc = QJsonDocument::fromJson(response);
    qDebug()<<doc.isNull();
    if(!doc.isNull())
    {
        QJsonObject obj = doc.object();
        if(obj.value("code").toInt() == 200)
        {
            qDebug()<<"response success code: "<<obj.value("code").toInt();
            emit sendTimeInfoSuccessfully(curTimeList);
            curTimeList.clear();
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

void sendPowerOnOffWork::handleError(const QString &errorString)
{
    qDebug()<<"send error: "<<errorString;
}

void sendPowerOnOffWork::handleFail()
{
    qDebug()<<"重传次数过多";
}

void sendPowerOnOffWork::onCalculateSuccess()
{
    startSending(curTimeList);
}
