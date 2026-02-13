#include "sendlocalappinfowork.h"
#include "model/http/gettimestampswork.h"
#include "model/localappmonitor.h"
#include "model/myStruct.h"
#include "model/mysm4.h"
#include "globalVariables.h"
#include "model/databasemanager2.h"

#include <QThread>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

sendLocalAppInfoWork::sendLocalAppInfoWork(QObject *parent) : QObject(parent) {
    monitor = new LocalAppMonitor();
}

sendLocalAppInfoWork::~sendLocalAppInfoWork() {
    if(client) {
        delete client;
        client = nullptr;
    }
    
    if(monitor) {
        delete monitor;
        monitor = nullptr;
    }
}

void sendLocalAppInfoWork::recheckTimestamps() {
    static unsigned short count = 0;
    qDebug() << "count: " << count;
    if(count > 1) {
        qDebug() << "Too many recheck attempts, terminating";
        return;
    }
    
    getTimestampsWork* tmpTimestampsWork = new getTimestampsWork();
    QThread* tmpGetTimestampsThread = new QThread();
    tmpTimestampsWork->moveToThread(tmpGetTimestampsThread);
    connect(tmpGetTimestampsThread, &QThread::finished, tmpTimestampsWork, &QObject::deleteLater);
    connect(this, &sendLocalAppInfoWork::startGetTimestamps, tmpTimestampsWork, &getTimestampsWork::startSending);
    connect(tmpTimestampsWork, &getTimestampsWork::calculateSuccess, this, &sendLocalAppInfoWork::onCalculateSuccess);
    connect(tmpTimestampsWork, &getTimestampsWork::workFinished, tmpGetTimestampsThread, &QThread::quit);
    
    tmpGetTimestampsThread->start();
    emit startGetTimestamps(urlCommon + "/time");
    
    count++;
}

void sendLocalAppInfoWork::startSending() {
    if(client) {
        delete client;
        client = nullptr;
    }
    
    databaseManager2 dbManager2;
    dbManager2.connectToDatabase();
    
    QString username;
    QString department;
    dbManager2.queryLatestLoginRecord(username, department);
    
    // 更新本地桌面应用记录
    monitor->updateLocalAppRecords();
    
    // 获取待发送的本地桌面应用记录
    QList<LocalAppRecord> pendingRecords = monitor->getPendingSendRecords();
    
    if (pendingRecords.isEmpty()) {
        qDebug() << "No local app records to send, skipping...";
        return;
    }
    
    // 构造 JSON 数据
    QJsonObject data;
    data["username"] = username;
    data["department"] = department;
    data["ip"] = fingerprint; // 使用全局变量中的 fingerprint 作为当前终端 IP
    
    QJsonArray localAppDataArray;
    for (const LocalAppRecord &record : pendingRecords) {
        QJsonObject localAppObj;
        localAppObj["localAppName"] = record.localAppName;
        localAppObj["status"] = record.status;
        localAppObj["startTime"] = record.startTime.toString("yyyy-MM-dd hh:mm:ss");
        localAppObj["endTime"] = record.endTime.toString("yyyy-MM-dd hh:mm:ss");
        localAppDataArray.append(localAppObj);
        
        qDebug() << "Adding local app record:" << record.localAppName;
        qDebug() << "  Status:" << record.status;
        qDebug() << "  Start:" << record.startTime.toString("yyyy-MM-dd hh:mm:ss");
        qDebug() << "  End:" << record.endTime.toString("yyyy-MM-dd hh:mm:ss");
    }
    
    data["localAppData"] = localAppDataArray;
    
    if (localAppDataArray.isEmpty()) {
        qDebug() << "No local app data to send, skipping...";
        return;
    }
    
    QJsonDocument doc = QJsonDocument(data);
    QByteArray docBytes = doc.toJson();
    
    mySm4 sm4;
    unsigned char *docData = reinterpret_cast<unsigned char*>(docBytes.data());
    unsigned long docLen = docBytes.size();
    QString encryptedStr;
    bool res = sm4.encryptStr(docData, docLen, encryptedStr);
    if(!res) {
        return;
    }
    
    QJsonObject root;
    root["method"] = "uploadLocalAppData";
    root["data"] = encryptedStr;
    
    client = new httpClient();
    connect(client, &httpClient::requestSuccess, this, &sendLocalAppInfoWork::handleSuccess);
    connect(client, &httpClient::requestError, this, &sendLocalAppInfoWork::handleError);
    connect(client, &httpClient::sendFail, this, &sendLocalAppInfoWork::handleFail);
    
    QUrl url = QUrl(urlCommon + "/uploadLocalAppData");
    client->post(url, QJsonDocument(root).toJson(QJsonDocument::Compact));
}

void sendLocalAppInfoWork::handleSuccess(const QByteArray &response) {
    QJsonDocument doc = QJsonDocument::fromJson(response);
    qDebug() << doc.isNull();
    if(!doc.isNull()) {
        QJsonObject obj = doc.object();
        if(obj.value("code").toInt() == 200) {
            qDebug() << "response success code: " << obj.value("code").toInt();
            // 发送成功，清除已发送的记录
            monitor->clearPendingSendRecords();
        }
        else if(obj.value("code").toInt() == 10005) {
            recheckTimestamps();
        }
        else {
            qDebug() << "response error code: " << obj.value("code").toInt();
        }
    }
}

void sendLocalAppInfoWork::handleError(const QString &errorString) {
    qDebug() << "Error sending local app info:" << errorString;
}

void sendLocalAppInfoWork::handleFail() {
    qDebug() << "Failed to send local app info";
}

void sendLocalAppInfoWork::onCalculateSuccess() {
    startSending();
}
