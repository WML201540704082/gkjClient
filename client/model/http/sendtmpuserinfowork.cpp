#include "sendtmpuserinfowork.h"
#include "model/mysm4.h"
#include "globalVariables.h"
#include "gettimestampswork.h"
#include "utf8.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QThread>

sendTmpUserInfoWork::sendTmpUserInfoWork(QObject *parent) : QObject(parent)
{
    
}

sendTmpUserInfoWork::~sendTmpUserInfoWork()
{
    if(client)
    {
        delete client;
        client = nullptr;
    }
}

void sendTmpUserInfoWork::recheckTimestamps()
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
    connect(this, &sendTmpUserInfoWork::startGetTimestamps, tmpTimestampsWork, &getTimestampsWork::startSending);
    connect(tmpTimestampsWork, &getTimestampsWork::calculateSuccess, this, &sendTmpUserInfoWork::onCalculateSuccess);
    connect(tmpTimestampsWork, &getTimestampsWork::workFinished, tmpGetTimestampsThread, &QThread::quit);
    
    tmpGetTimestampsThread->start();
    emit startGetTimestamps(urlCommon + "/time");
    
    count++;
}

void sendTmpUserInfoWork::processData(QJsonValue value)
{
    QList<tmpUserInfo> tmpUserList;
    qDebug()<<"value.toString().isEmpty() "<<value.toString().isEmpty();
    if(value.toString().isEmpty())
    {
        emit sendTmpUserInfoSuccessfully(tmpUserList, curTmpUserList);
        curTmpUserList.clear();
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
    
//    mySm4 sm4;
//    QByteArray decryptedDataBytes = QByteArray::fromHex(value.toString().toUtf8());
//    unsigned char *decryptedData = reinterpret_cast<unsigned char*>(decryptedDataBytes.data());
//    size_t decryptedTmpLen = decryptedDataBytes.size();
    
//    QString decryptedDataStr;
//    bool res = sm4.decrptStr(decryptedData, decryptedTmpLen, decryptedDataStr);
//    if(!res)
//    {
//        qDebug()<<"Failed to decrypt user pwd";
//        return;
//    }
    
    //生成需要同步到客户端的数据
    QJsonDocument dataDoc = QJsonDocument::fromJson(decryptedDataStr.toUtf8());
    QJsonObject dataObj = dataDoc.object();
    
    QJsonArray userList = dataObj["userList"].toArray();
    for(const QJsonValue &item : userList)
    {
        QJsonObject itemObj = item.toObject();
        tmpUserInfo tmpUser;
        tmpUser.type = itemObj["type"].toString();
        tmpUser.id = itemObj["id"].toString();
        tmpUser.username = itemObj["username"].toString();
        tmpUser.userPwd = itemObj["userPwd"].toString();
        tmpUser.faceData = itemObj["faceData"].toString();
        tmpUser.department = itemObj["department"].toString();
        tmpUser.operationTime = itemObj["operationTime"].toString();
        
        tmpUserList.append(tmpUser);
    }
  
    emit sendTmpUserInfoSuccessfully(tmpUserList, curTmpUserList);
    curTmpUserList.clear();
}

void sendTmpUserInfoWork::startSending(const QList<tmpUserInfo> &tmpUserList)
{
    curTmpUserList = tmpUserList;
    
    if(client)
    {
        delete client;
        client = nullptr;
    }
    
    //拼接json
    QJsonArray tmpUserInfoArray;
    for(auto tmpUser : tmpUserList)
    {
        QJsonObject tmpUserInfoObj;
        
        tmpUserInfoObj["type"] = tmpUser.type;
        tmpUserInfoObj["id"] = tmpUser.id;
        tmpUserInfoObj["username"] = tmpUser.username;
        tmpUserInfoObj["userPwd"] = tmpUser.userPwd;
        tmpUserInfoObj["faceData"] = tmpUser.faceData;
        tmpUserInfoObj["department"] = tmpUser.department;
        tmpUserInfoObj["operationTime"] = tmpUser.operationTime;
        
        tmpUserInfoArray.append(tmpUserInfoObj);
    }
    
    QJsonObject data;
    data["userList"] = tmpUserInfoArray;
    
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
    root["method"] = "uploadTemp";
    root["data"] = encryptedStr;
    
    //初始化client并发送
    client = new httpClient();
    connect(client, &httpClient::requestSuccess, this, &sendTmpUserInfoWork::handleSuccess);
    connect(client, &httpClient::requestError, this, &sendTmpUserInfoWork::handleError);
    connect(client, &httpClient::sendFail, this, &sendTmpUserInfoWork::handleFail);
    
    QUrl url = QUrl(urlCommon + "/uploadUser");
    client->post(url, QJsonDocument(root).toJson(QJsonDocument::Compact));
}

void sendTmpUserInfoWork::handleSuccess(const QByteArray &response)
{
    QJsonDocument doc = QJsonDocument::fromJson(response);
    qDebug()<<"doc "<<doc;
    if(!doc.isNull())
    {
        QJsonObject obj = doc.object();
        if(obj.value("code").toInt() == 200)
        {
            qDebug()<<"response success code: "<<obj.value("code").toInt();
            processData(obj.value("data"));
//            emit sendTmpUserInfoSuccessfully(curTmpUserList);
//            curTmpUserList.clear();
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

void sendTmpUserInfoWork::handleError(const QString &errorString)
{
    qDebug()<<"send error: "<<errorString;
}

void sendTmpUserInfoWork::handleFail()
{
    qDebug()<<"重传次数过多";
}

void sendTmpUserInfoWork::onCalculateSuccess()
{
    startSending(curTmpUserList);
}
