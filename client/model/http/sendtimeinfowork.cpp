#include "sendtimeinfowork.h"
#include "model/mysm4.h"
#include "globalVariables.h"
#include "gettimestampswork.h"
#include "utf8.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QThread>
#include <QDebug>

sendTimeInfoWork::sendTimeInfoWork(QObject *parent) : QObject(parent)
{
    
}

sendTimeInfoWork::~sendTimeInfoWork()
{
    if(client)
    {
        delete client;
        client = nullptr;
    }
}

void sendTimeInfoWork::recheckTimestamps()
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
    connect(this, &sendTimeInfoWork::startGetTimestamps, tmpTimestampsWork, &getTimestampsWork::startSending);
    connect(tmpTimestampsWork, &getTimestampsWork::calculateSuccess, this, &sendTimeInfoWork::onCalculateSuccess);
    connect(tmpTimestampsWork, &getTimestampsWork::workFinished, tmpGetTimestampsThread, &QThread::quit);
    
    tmpGetTimestampsThread->start();
    emit startGetTimestamps(urlCommon + "/time");
    
    count++;
}

void sendTimeInfoWork::processData(QJsonValue value)
{
    QList<usageTime> serverTimeList;
    if(value.toString().isEmpty())
    {
        emit sendUsageTimeInfoSuccessfully(serverTimeList, curUsageTimeList);
        curUsageTimeList.clear();
        return;
    }
    
    //先解密data
    QString strServerKey = "5f8d7e6c4b3a2918070654321fedcba9";
    QByteArray keyBytes = QByteArray::fromHex(strServerKey.toUtf8()); 
    uint8_t server_key[16];
    memcpy(server_key, keyBytes.constData(), 16);
    
    QString strServerIv = "8a7b6c5d4e3f2a1b0c9d8e7f6a5b4c3d";
    QByteArray ivBytes = QByteArray::fromHex(strServerIv.toUtf8()); 
    uint8_t server_iv[16];
    memcpy(server_iv, ivBytes.constData(), 16);
    
    SM4_KEY encrypt_key;
    SM4_KEY decrypt_key; 
    sm4_set_encrypt_key(&encrypt_key, server_key);
    sm4_set_decrypt_key(&decrypt_key, server_key);
    
    QByteArray encryptedDataBytes = QByteArray::fromHex(value.toString().toUtf8());
    unsigned char *encryptedData = reinterpret_cast<unsigned char*>(encryptedDataBytes.data());
    size_t encryptedTmpLen = encryptedDataBytes.size();
    
    // 为解密后的数据分配内存
    uint8_t *decryptedData = (uint8_t *)malloc(encryptedTmpLen);  // 解密后的数据大小不会超过加密数据的大小
    if (decryptedData == nullptr)
    {
        qDebug() << "Memory allocation failed for decrypted data!";
        return;
    }
    
    size_t decryptedLen = 0;
    
    int ret = sm4_cbc_padding_decrypt(&decrypt_key, server_iv, encryptedData, encryptedTmpLen, decryptedData, &decryptedLen);
    if(ret != 1)
    {
        qDebug() << "Failed to decrypt data.";
        free(decryptedData);
        return;
    }
    
    QByteArray byteArray(reinterpret_cast<const char *>(decryptedData), decryptedLen);
    QString decryptedDataStr = QString::fromUtf8(byteArray);
    qDebug()<<"decryptedDataStr: "<<decryptedDataStr;
    
    free(decryptedData);
    
    //生成需要同步到客户端的数据
    QJsonDocument dataDoc = QJsonDocument::fromJson(decryptedDataStr.toUtf8());
    QJsonObject dataObj = dataDoc.object();
     
    QJsonArray timeList = dataObj["time"].toArray();
    for(const QJsonValue &item : timeList)
    {
        QJsonObject itemObj = item.toObject();
        usageTime time;
        time.type = itemObj["type"].toString();
        time.id = itemObj["id"].toString();
        time.userId = itemObj["userId"].toString();
        time.startTime = itemObj["startTime"].toString();
        time.endTime = itemObj["endTime"].toString();
        time.operationTime = itemObj["operationTime"].toString();
        
        serverTimeList.append(time);
    }
    
    emit sendUsageTimeInfoSuccessfully(serverTimeList, curUsageTimeList);
    curUsageTimeList.clear();
}

void sendTimeInfoWork::startSending(const QList<usageTime> &usageTimeList)
{
    curUsageTimeList = usageTimeList;
//    for(auto item : usageTimeList)
//    {
//        qDebug()<<item.userId<<" "<<item.startTime<<" "<<item.endTime;
//    }
    
    if(client)
    {
        delete client;
        client = nullptr;
    }
    
    //拼接json
    QJsonArray usageTimeArray;
    for(auto usageTime : usageTimeList)
    {
        QJsonObject usageTimeInfoObj;
        
        usageTimeInfoObj["type"] = usageTime.type;
        usageTimeInfoObj["id"] = usageTime.id;
        usageTimeInfoObj["userId"] = usageTime.userId;
        usageTimeInfoObj["startTime"] = usageTime.startTime;
        usageTimeInfoObj["endTime"] = usageTime.endTime;
        usageTimeInfoObj["operationTime"] = usageTime.operationTime;
        
        usageTimeArray.append(usageTimeInfoObj);
    }
    
    QJsonObject data;
    data["time"] = usageTimeArray;
    
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
    root["method"] = "uploadTime";
    root["data"] = encryptedStr;
    
    //初始化client并发送
    client = new httpClient();
    connect(client, &httpClient::requestSuccess, this, &sendTimeInfoWork::handleSuccess);
    connect(client, &httpClient::requestError, this, &sendTimeInfoWork::handleError);
    connect(client, &httpClient::sendFail, this, &sendTimeInfoWork::handleFail);
    
    QUrl url = QUrl(urlCommon + "/uploadTime");
    client->post(url, QJsonDocument(root).toJson(QJsonDocument::Compact));
}

void sendTimeInfoWork::handleSuccess(const QByteArray &response)
{
    QJsonDocument doc = QJsonDocument::fromJson(response);
    if(!doc.isNull())
    {
        QJsonObject obj = doc.object();
        if(obj.value("code").toInt() == 200)
        {
            qDebug()<<"response success code: "<<obj.value("code").toInt();
            processData(obj.value("data"));
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

void sendTimeInfoWork::handleError(const QString &errorString)
{
    qDebug()<<"send error: "<<errorString;
}

void sendTimeInfoWork::handleFail()
{
    qDebug()<<"重传次数过多";
}

void sendTimeInfoWork::onCalculateSuccess()
{
    startSending(curUsageTimeList);
}
