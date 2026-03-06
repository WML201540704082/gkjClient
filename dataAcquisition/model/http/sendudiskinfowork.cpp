#include "sendudiskinfowork.h"
#include "model/udiskmonitor.h"
#include "model/http/gettimestampswork.h"
#include "model/myStruct.h"
#include "model/mysm4.h"
#include "globalVariables.h"
#include "model/databasemanager2.h"
#include "model/http/httpclient.h"

#include <QThread>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

sendUDiskInfoWork::sendUDiskInfoWork(QObject *parent) : QObject(parent) {
    monitor = new UDiskMonitor();
}

sendUDiskInfoWork::~sendUDiskInfoWork() {
    if(client) {
        delete client;
        client = nullptr;
    }

    if(monitor) {
        delete monitor;
        monitor = nullptr;
    }
}

void sendUDiskInfoWork::recheckTimestamps() {
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
    connect(this, &sendUDiskInfoWork::startGetTimestamps, tmpTimestampsWork, &getTimestampsWork::startSending);
    connect(tmpTimestampsWork, &getTimestampsWork::calculateSuccess, this, &sendUDiskInfoWork::onCalculateSuccess);
    connect(tmpTimestampsWork, &getTimestampsWork::workFinished, tmpGetTimestampsThread, &QThread::quit);

    tmpGetTimestampsThread->start();
    emit startGetTimestamps(urlCommon + "/time");

    count++;
}

void sendUDiskInfoWork::startSending() {
    if(client) {
        delete client;
        client = nullptr;
    }

    databaseManager2 dbManager2;
    dbManager2.connectToDatabase();

    QString username;
    QString department;
    dbManager2.queryLatestLoginRecord(username, department);

    // 更新本地U盘记录
    monitor->updateUDiskRecords();

    // 获取待发送的本地U盘记录
    QList<UDiskRecord> list = monitor->getPendingSendRecords();

    if (list.isEmpty()) {
        qDebug() << "No UDisk records to send, skipping...";
        return;
    }

    // 构造 JSON 数据
    QJsonObject data;
    data["username"] = username;
    data["department"] = department;
    data["ip"] = fingerprint; // 使用全局变量中的 fingerprint 作为当前终端 IP

    QJsonArray UDiskDataArray;
    for (int i = 0; i < list.size(); i++) {
        UDiskRecord item = list.at(i);
        QJsonObject UDiskObj;
        UDiskObj["UDiskName"] = item.UDiskName;
        UDiskObj["isEncryptedUDisk"] = item.isEncryptedUDisk;
        UDiskObj["startTime"] = item.startTime.toString("yyyy-MM-dd hh:mm:ss");
        UDiskObj["endTime"] = item.endTime.toString("yyyy-MM-dd hh:mm:ss");
        UDiskDataArray.append(UDiskObj);

        qDebug() << "Adding UDisk record:" << item.UDiskName;
        qDebug() << "  Is encrypted UDisk:" << item.isEncryptedUDisk;
        qDebug() << "  Start:" << item.startTime.toString("yyyy-MM-dd hh:mm:ss");
        qDebug() << "  End:" << item.endTime.toString("yyyy-MM-dd hh:mm:ss");
    }

    data["UDiskData"] = UDiskDataArray;

    if (UDiskDataArray.isEmpty()) {
        qDebug() << "No UDisk data to send, skipping...";
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
    root["method"] = "uploadUDiskData";
    root["data"] = encryptedStr;

    client = new httpClient();
    connect(client, &httpClient::requestSuccess, this, &sendUDiskInfoWork::handleSuccess);
    connect(client, &httpClient::requestError, this, &sendUDiskInfoWork::handleError);
    connect(client, &httpClient::sendFail, this, &sendUDiskInfoWork::handleFail);

    QUrl url = QUrl(urlCommon + "/uploadUDiskData");
    client->post(url, QJsonDocument(root).toJson(QJsonDocument::Compact));
}

void sendUDiskInfoWork::handleSuccess(const QByteArray &response) {
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

void sendUDiskInfoWork::handleError(const QString &errorString) {
    qDebug() << "Error sending UDisk info:" << errorString;
}

void sendUDiskInfoWork::handleFail() {
    qDebug() << "Failed to send UDisk info";
}

void sendUDiskInfoWork::onCalculateSuccess() {
    startSending();
}
