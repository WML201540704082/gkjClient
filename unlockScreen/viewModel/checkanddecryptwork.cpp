#include "checkanddecryptwork.h"
#include "utf8.h"

#include <QFile>
#include <QSettings>
#include <QDebug>
#include <QMetaType>

extern std::atomic<bool> decryptDone;

checkAndDecryptWork::checkAndDecryptWork(QObject *parent) : QObject(parent)
{
    
}

void checkAndDecryptWork::onStartCheckAndDecrypt()
{
    qDebug()<<"onStartCheckAndDecrypt()";
    qRegisterMetaType<size_t>("size_t");
    //先判断是否注册了管理员权限，没有则直接发送信号退出程序
    databaseManager dbManager;
    
    QString adminUuid;
    QString adminImagePath;
    QString username;
    QString userPwd;
    QString adminDepartment;
    
    dbManager.connectToDatabase();
    dbManager.queryAdminData(adminUuid, username, userPwd, adminImagePath, adminDepartment);
    
    if(adminUuid.isEmpty() || username.isEmpty() || userPwd.isEmpty() || adminImagePath.isEmpty() || adminDepartment.isEmpty())
    {
        emit threadErrorFinished("没有注册人脸信息");
        return;
    }
    //已注册管理员权限则判断是否允许临时用户登录
    else
    { 
        tmpUserLogin = dbManager.queryTempUserLogin();
    }
    
    //解密管理员密码
    pSm4 = new mySm4();
    QByteArray encryptedBytes = QByteArray::fromHex(userPwd.toUtf8());
    unsigned char *encryptedData = reinterpret_cast<unsigned char*>(encryptedBytes.data());
    size_t encryptedLen = encryptedBytes.size();
    
    QString decryptedPwd;
    bool ret1 = pSm4->decrptStr(encryptedData, encryptedLen, decryptedPwd);
    qDebug()<<"decryptedPwd "<<decryptedPwd;    
    if(!ret1)
    {
        qDebug()<<"Failed to decrypt admin pwd";
        emit threadErrorFinished("解密管理员密码出错");
        return;
    }
    
    //解密管理员图像数据
//    pSm4 = new mySm4();
    uint8_t *encryptedAdminData = nullptr;
    size_t encryptedAdminDataLen = 0;
    
    uint8_t *decryptedAdminData = nullptr;
    size_t decrypted_len = 0;
    
    if(!pSm4->decrypt(adminImagePath, encryptedAdminData, encryptedAdminDataLen,
                      decryptedAdminData, decrypted_len))
    {
        qDebug()<<"Failed to decrypt admin data";
        emit threadErrorFinished("解密管理员人脸数据出错");
        return;
    }
    
    //剔除uuid
    size_t adminUuidLen = adminUuid.size();
    size_t adminImageDataLen = decrypted_len - adminUuidLen;
    unsigned char *adminImageData = new unsigned char[adminImageDataLen];
    std::copy(decryptedAdminData + adminUuidLen, decryptedAdminData  + adminUuidLen + adminImageDataLen, adminImageData);   
    //只保留解密后的图片数据
    if(decryptedAdminData != nullptr)
    {
        free(decryptedAdminData);
        decryptedAdminData = nullptr;
    }
    
    //允许临时用户登录
    if(tmpUserLogin)
    {
        QList<QList<QString>> tmpUsersData;
        bool res = dbManager.queryAvailableTmpUserData(tmpUsersData);
        //允许临时用户登录但没有有效期内的临时用户
        if(!res)
        {
            if(pSm4 != nullptr)
            {
                delete pSm4;
                pSm4 = nullptr;
            }
            qDebug()<<"解密完成（允许临时用户登录但没有有效临时用户）";
            emit sendAdminData(adminImageData, adminImageDataLen, username, decryptedPwd, adminDepartment);
            return;
        }
        //解密临时用户图像数据
        else
        {
            QList<std::tuple<QString, unsigned char *, size_t, QString, QString, QString>> decryptedTmpUserList;
            for(const QList<QString> &tmpUser : tmpUsersData)
            {
                QString tmpUserUuid = tmpUser[0];
                QString tmpUsername = tmpUser[1];
                QString tmpUserImagePath = tmpUser[2];
                QString tmpUserPwd = tmpUser[3];
                QString tmpUserDepartment = tmpUser[4];
                
                uint8_t *encryptedTmpUserData = nullptr;
                size_t encryptedTmpUserDataLen = 0;
                
                uint8_t *decryptedTmpUserData = nullptr;
                size_t decryptedTmpUserDataLen = 0;
                
                if(!pSm4->decrypt(tmpUserImagePath, encryptedTmpUserData, encryptedTmpUserDataLen,
                                  decryptedTmpUserData, decryptedTmpUserDataLen))
                {
                    qDebug()<<"Failed to decrypt temp user data";
                    emit threadErrorFinished("解密临时授权用户人脸数据出错");
                    return;
                }
                //剔除uuid
                size_t tmpUserUuidLen = tmpUserUuid.size();
                size_t tmpUserImageDataLen = decryptedTmpUserDataLen - tmpUserUuidLen;
                unsigned char *tmpUserImageData = new unsigned char[tmpUserImageDataLen];
                std::copy(decryptedTmpUserData + tmpUserUuidLen, decryptedTmpUserData  + tmpUserUuidLen + tmpUserImageDataLen, tmpUserImageData);   
                //只保留解密后的图片数据
                if(decryptedTmpUserData != nullptr)
                {
                    free(decryptedTmpUserData);
                    decryptedTmpUserData = nullptr;
                }
                
//                std::tuple<QString, unsigned char *, size_t> tmpUserTuple(tmpUserUuid, tmpUserImageData, tmpUserImageDataLen);
//                QMap<QString, unsigned char *> decryptTmpUerMap;
//                decryptTmpUerMap.insert(tmpUserUuid, tmpUserImageData);
                
                //解密临时用户密码
                QByteArray decryptedTmpBytes = QByteArray::fromHex(tmpUserPwd.toUtf8());
                unsigned char *decryptedTmpData = reinterpret_cast<unsigned char*>(decryptedTmpBytes.data());
                size_t decryptedTmpLen = decryptedTmpBytes.size();
                
                QString decryptedTmpPwd;
                bool ret2 = pSm4->decrptStr(decryptedTmpData, decryptedTmpLen, decryptedTmpPwd);
                if(!ret2)
                {
                    qDebug()<<"Failed to decrypt temp user pwd";
                    emit threadErrorFinished("解密临时授权用户密码出错");
                    return;
                }
                
                std::tuple<QString, unsigned char *, size_t, QString, QString, QString> tmpUserTuple(
                            tmpUserUuid, tmpUserImageData, tmpUserImageDataLen, tmpUsername, decryptedTmpPwd, tmpUserDepartment);

                decryptedTmpUserList.append(tmpUserTuple);
            }
            if(pSm4 != nullptr)
            {
                delete pSm4;
                pSm4 = nullptr;
            }
            qDebug()<<"解密完成（允许临时用户登录）";
            emit sendTmpUserData(adminImageData, adminImageDataLen, username,
                                 decryptedPwd, adminDepartment, decryptedTmpUserList);
        }      
    }
    //不允许临时用户登录则发送数据结束线程操作
    else
    {        
        if(pSm4 != nullptr)
        {
            delete pSm4;
            pSm4 = nullptr;
        }
        qDebug()<<"解密完成（不允许临时用户登录）";
        emit sendAdminData(adminImageData, adminImageDataLen, username, decryptedPwd,adminDepartment);
    }   
}
