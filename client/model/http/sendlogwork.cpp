#include "sendlogwork.h"
#include "model/mysm4.h"
#include "globalVariables.h"
#include "gettimestampswork.h"
#include "utf8.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QThread>

sendLogWork::sendLogWork(QObject *parent) : QObject(parent)
{
    
}

sendLogWork::~sendLogWork()
{
    if(client)
    {
        delete client;
        client = nullptr;
    }
}

void sendLogWork::recheckTimestamps()
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
    connect(this, &sendLogWork::startGetTimestamps, tmpTimestampsWork, &getTimestampsWork::startSending);
    connect(tmpTimestampsWork, &getTimestampsWork::calculateSuccess, this, &sendLogWork::onCalculateSuccess);
    connect(tmpTimestampsWork, &getTimestampsWork::workFinished, tmpGetTimestampsThread, &QThread::quit);
    
    tmpGetTimestampsThread->start();
    emit startGetTimestamps(urlCommon + "/time");
    
    count++;
}

void sendLogWork::startSending(const QList<loginInfo> &logList)
{
    qDebug()<<"sendLogWork::startSending";
    curLogList = logList;
    
    if(client)
    {
        delete client;
        client = nullptr;
    }
    
    //拼接json
    QJsonArray logInfoArray;
    for(auto logInfo : logList)
    {
        QJsonObject logInfoObj;
        
        logInfoObj["username"] = logInfo.username;
        logInfoObj["loginTime"] = logInfo.loginTime;
        logInfoObj["logoutTime"] = logInfo.logoutTime;
        logInfoObj["department"] = logInfo.department;
        
        logInfoArray.append(logInfoObj);
    }
    
    QJsonObject data;
    data["logList"] = logInfoArray;
    
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
    root["method"] = "log";
    root["data"] = encryptedStr;
    
    //初始化client并发送
    client = new httpClient();
    connect(client, &httpClient::requestSuccess, this, &sendLogWork::handleSuccess);
    connect(client, &httpClient::requestError, this, &sendLogWork::handleError);
    connect(client, &httpClient::sendFail, this, &sendLogWork::handleFail);
    
    QUrl url = QUrl(urlCommon + "/uploadLog");
    client->post(url, QJsonDocument(root).toJson(QJsonDocument::Compact));
}

void sendLogWork::handleSuccess(const QByteArray &response)
{
    QJsonDocument doc = QJsonDocument::fromJson(response);
    qDebug()<<doc.isNull();
    if(!doc.isNull())
    {
        QJsonObject obj = doc.object();
        if(obj.value("code").toInt() == 200)
        {
            qDebug()<<"response success code: "<<obj.value("code").toInt();
            emit sendLogInfoSuccessfully(curLogList);
            curLogList.clear();
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

void sendLogWork::handleError(const QString &errorString)
{
    qDebug()<<"send error: "<<errorString;
}

void sendLogWork::handleFail()
{
    qDebug()<<"重传次数过多";
}

void sendLogWork::onCalculateSuccess()
{
    startSending(curLogList);
}
