#include "httpclient.h"
#include "globalVariables.h"
#include "model/sm3/sm3.h"

#include "model/sm4/sm4.h"
#include "model/sm4/hex.h"
#include "model/sm4/rand.h"
#include "model/sm4/error.h"

#include <QNetworkRequest>
#include <QDebug>
#include <QUuid>
#include <QDateTime>
#include <string>

using namespace std;

httpClient::httpClient(QObject *parent) : QObject(parent), retryCount(0), maxRetries(0), reply(nullptr), timeoutTimer(nullptr)
{
    manager = new QNetworkAccessManager(this);
    manager->setNetworkAccessible(QNetworkAccessManager::Accessible);
    connect(manager, &QNetworkAccessManager::finished, this, &httpClient::onFinished);
}

void httpClient::get(const QUrl &url)
{
    this->url = url;
    retryCount = 0;
    makeGetRequest();
}

void httpClient::post(const QUrl &url, const QByteArray &data)
{
    this->url = url;
    this->data = data;
    retryCount = 0;
    makePostRequest();
}

void httpClient::onFinished(QNetworkReply *reply)
{
    if(timeoutTimer)
    {
        timeoutTimer->stop();
    }
    if(reply->error() == QNetworkReply::NoError && reply->isOpen())
    {
        QByteArray responseData = reply->readAll();
        qDebug() << "responseData: " << responseData;
        emit requestSuccess(responseData);
    }
    else
    {
//        QByteArray responseData = reply->readAll();
//        qDebug()<<"error response: "<<QString::fromLocal8Bit(responseData);
        QString errorString = reply->errorString();
        qDebug()<< "error: " << errorString;
        emit requestError(errorString);
        if(retryCount < maxRetries)
        {
            retryCount ++;
            qDebug() << "retrying..." << retryCount;
            setupTimeout();
            if(data.isEmpty())
            {
                makeGetRequest();
            }
            else
            {
                makePostRequest();
            }
            return;
        }
        else if(retryCount == maxRetries)
        {
            qDebug() << "retrying too many times... " << retryCount;
            emit sendFail();
        }
    }
    reply->deleteLater();
}

void httpClient::onTimeout()
{
    if(reply)
    {
        qDebug() << "request timed out";
        
        //防止触发finished信号和onFinished槽函数
        disconnect(reply, nullptr, this, nullptr);
        
        reply->abort();       //中断请求
        reply->deleteLater(); //释放资源
        reply = nullptr;
        
        if(retryCount < maxRetries)
        {
            retryCount ++;
            qDebug() << "retrying due to timeout... " << retryCount;
            if(data.isEmpty())
            {
                makeGetRequest();
            }
            else
            {
                makePostRequest();
            }
        }
    }
}

void httpClient::makeGetRequest()
{
    QNetworkRequest request(url);
    reply = manager->get(request);
    setupTimeout();
}

void httpClient::makePostRequest()
{
    //先判断指纹是否获取
    if(fingerprint.isEmpty())
    {
        qDebug()<<"未能正确获取指纹信息";
        return;
    }
//    fingerprint.remove(".");
    QUuid uuid = QUuid::createUuid();
    QString uuidStr = uuid.toString(QUuid::WithoutBraces);
    uuidStr.remove("-");
    
    QByteArray uuidBytes = uuid.toRfc4122();
    uint8_t uuid_key[16];
    memcpy(uuid_key, uuidBytes.constData(), 16);
    const uint8_t iv[16] = {0xAD, 0x01, 0xC2, 0xB3, 0x64, 0x21, 0xCC, 0x77,
                            0xFF, 0x12, 0x0A, 0x0D, 0xEC, 0x7B, 0x0E, 0x0F};
    
    SM4_KEY encrypt_key;
    SM4_KEY decrypt_key;
    
    sm4_set_encrypt_key(&encrypt_key, uuid_key);
    sm4_set_decrypt_key(&decrypt_key, uuid_key);
    
    QString encryptedFingerprint;
    QByteArray fingerprintBytes = fingerprint.toUtf8();
    unsigned char *fingerprintData = reinterpret_cast<unsigned char*>(fingerprintBytes.data());
    unsigned long fingerprintDataLen = fingerprintBytes.size();  // 不包含结尾的 '\0'
    
    uint8_t *encryptedData = (uint8_t *)malloc(fingerprintDataLen + SM4_BLOCK_SIZE);  // 为加密数据预留空间
    if (encryptedData == nullptr) 
    {
        qDebug() << "Memory allocation failed for encrypted data!";
    }
    
    size_t encrypted_len = 0;
    // 对数据进行加密
    int ret = sm4_cbc_padding_encrypt(&encrypt_key, iv, fingerprintData, fingerprintDataLen, encryptedData, &encrypted_len);
    if(ret != 1)
    {
        qDebug() << "Failed to encrypt image.";
    }
    
    QByteArray byteArray(reinterpret_cast<const char*>(encryptedData), encrypted_len);
    encryptedFingerprint = byteArray.toHex();
    qDebug()<<encryptedFingerprint;
 
    // 释放分配的内存
    free(encryptedData);
    
//    string uuidKey = uuidStr.toStdString();
//    string encodeText = sm4.sm4encodestrhex(fingerprint.toStdString(), uuidKey);
//    QString fingerPrint = QString::fromStdString(encodeText);
    QString fingerPrint = encryptedFingerprint;
    
    //时间戳
//    static qint64 tmp = 0;
    qint64 timestamps = QDateTime::currentSecsSinceEpoch() + timeDifference /*+ tmp*/;
//    tmp+=100000;    
    
    //json(param)
    QString dataStr = QString(data);
    
    //拼接
    QString fullStr = fingerprint + uuidStr + QString::number(timestamps) + dataStr;
    QByteArray fullBytes = fullStr.toUtf8();
    
    //sm3加密
    SM3_CTX sm3_ctx;
    uint8_t dgst[SM3_DIGEST_SIZE];
    sm3_init(&sm3_ctx);
    sm3_update(&sm3_ctx, (uint8_t *)(fullBytes.constData()), fullBytes.size());
    sm3_finish(&sm3_ctx, dgst);
    
    //签名
    QByteArray sign(reinterpret_cast<const char *>(dgst), SM3_DIGEST_SIZE);
    qDebug()<<"sign: "<<sign.toHex();
    
    if(uuidStr.isEmpty() || fingerPrint.isEmpty() || timestamps == 0 || sign.isEmpty())
    {
        qDebug()<<"未能正确生成http请求头所需信息";
        return;
    }
    
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("nonce", uuidStr.toLocal8Bit());
    request.setRawHeader("fingerprint", fingerPrint.toLocal8Bit());
    request.setRawHeader("ts", QByteArray::number(timestamps));
    request.setRawHeader("sign", sign.toHex());
//    qDebug()<<"post";
//    qDebug()<<"url: "<<url.toString();
//    qDebug()<<"header: "<<request.rawHeaderList();
//    qDebug()<<"body: "<<data;
    reply = manager->post(request, data);
    setupTimeout();
}

void httpClient::setupTimeout()
{
    if(timeoutTimer)
    {
        timeoutTimer->stop();
        timeoutTimer->deleteLater();
    }
    
    timeoutTimer = new QTimer(this);
    timeoutTimer->setSingleShot(true);
    connect(timeoutTimer, &QTimer::timeout, this, &httpClient::onTimeout);
    timeoutTimer->start(5000);
}

