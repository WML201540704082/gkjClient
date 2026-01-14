#include "syncwork.h"
#include "globalVariables.h"
#include "model/databasemanager.h"
#include "model/mysm4.h"

#include <QDebug>
#include <QMetaType>
#include <QCoreApplication>
#include <QDir>

syncWork::syncWork(QObject *parent) : QObject(parent)
{
    
}

void syncWork::sendSyncData()
{
    //先获取需要同步的数据
    databaseManager dbManager;
    dbManager.connectToDatabase();
    QList<QList<QString>> recordsList;
    bool res = dbManager.getSimplifiedRecord(recordsList);
    qDebug()<<recordsList.size();
    
    QList<adminInfo> adminList;
    QList<tmpUserInfo> tmpUserList;
    QList<usageTime> timeList;
    QList<loginInfo> loginInfoList;
            
    for(auto record : recordsList)
    {
        QString tableName = record[0];
        QString recordId = record[1];
        QString operationType = record[2];
        QString operationTime = record[3];
        qDebug()<<tableName<<" "<<recordId<<" "<<operationType<<" "<<operationTime;
        
        if(tableName == "Admin" && operationType == "add")
        {
            adminInfo admin;
            
            QString uuid;
            QString username;
            QString userPwd;
            QString faceImagePath;
            QString department;
            
            dbManager.queryAdminData(uuid, username, userPwd, faceImagePath, department);
            if(recordId != uuid)
            {
                continue;
            }
            
            //能查到id就解密图片数据
            mySm4 sm4;
            uint8_t *encryptedAdminData = nullptr;
            size_t encryptedAdminDataLen = 0;
            
            uint8_t *decryptedAdminData = nullptr;
            size_t decrypted_len = 0;
            
            if(!sm4.decrypt(faceImagePath, encryptedAdminData, encryptedAdminDataLen,
                              decryptedAdminData, decrypted_len))
            {
                qDebug()<<"Failed to decrypt admin data before sync";
                return;
            }
            
            //剔除uuid
            size_t adminUuidLen = uuid.size();
            size_t adminImageDataLen = decrypted_len - adminUuidLen;
            unsigned char *adminImageData = new unsigned char[adminImageDataLen];
            std::copy(decryptedAdminData + adminUuidLen, decryptedAdminData  + adminUuidLen + adminImageDataLen, adminImageData);   
            
            QByteArray byteArray(reinterpret_cast<char*>(adminImageData), adminImageDataLen);
            
            //解密密码
            QByteArray decryptedTmpBytes = QByteArray::fromHex(userPwd.toUtf8());
            unsigned char *decryptedTmpData = reinterpret_cast<unsigned char*>(decryptedTmpBytes.data());
            size_t decryptedTmpLen = decryptedTmpBytes.size();
            
            QString decryptedUserPwd;
            bool res2 = sm4.decrptStr(decryptedTmpData, decryptedTmpLen, decryptedUserPwd);
            if(!res2)
            {
                qDebug()<<"Failed to decrypt user pwd";
            }
            
            admin.type = operationType;
            admin.id = uuid;
            admin.username = username;
            admin.userPwd = decryptedUserPwd;
            admin.department = department;
            admin.faceData = byteArray.toBase64();
            admin.operationTime = operationTime;
            adminList.append(admin);
            
            if(decryptedAdminData != nullptr)
            {
                free(decryptedAdminData);
                decryptedAdminData = nullptr;
            }
            if(adminImageData != nullptr)
            {
                delete[] adminImageData;
                adminImageData = nullptr;
            }
        }
        else if(tableName == "Admin" && operationType == "delete")
        {
            adminInfo admin;
            admin.type = operationType;
            admin.id = recordId;
            admin.operationTime = operationTime;
            
            adminList.append(admin);
        }
        else if(tableName == "TemporaryUser" && (operationType == "add" || operationType == "update"))
        {
            tmpUserInfo tmpUser;
            
            QString uuid = recordId;
            QList<QString> tmpUserInfo;
            QString username;
            QString userPwd;
            QString faceImagePath;
            QString department;
            
            bool res = dbManager.queryTempUserDataById(uuid, tmpUserInfo);
            if(!res || tmpUserInfo.isEmpty())
            {
                continue;
            }
            if(tmpUserInfo.size() == 4)
            {
                username = tmpUserInfo[0];
                userPwd = tmpUserInfo[1];
                faceImagePath = tmpUserInfo[2];
                department = tmpUserInfo[3];
            }
            
            //解密密码
            mySm4 sm4;
            QByteArray decryptedTmpBytes = QByteArray::fromHex(userPwd.toUtf8());
            unsigned char *decryptedTmpData = reinterpret_cast<unsigned char*>(decryptedTmpBytes.data());
            size_t decryptedTmpLen = decryptedTmpBytes.size();
            
            QString decryptedUserPwd;
            bool res2 = sm4.decrptStr(decryptedTmpData, decryptedTmpLen, decryptedUserPwd);
            if(!res2)
            {
                qDebug()<<"Failed to decrypt user pwd";
            }
            
            //解密图片
            uint8_t *encryptedTmpUserData = nullptr;
            size_t encryptedTmpUserDataLen = 0;
            
            uint8_t *decryptedTmpUserData = nullptr;
            size_t decryptedTmpUserDataLen = 0;
            
            if(!sm4.decrypt(faceImagePath, encryptedTmpUserData, encryptedTmpUserDataLen,
                              decryptedTmpUserData, decryptedTmpUserDataLen))
            {
                qDebug()<<"Failed to decrypt temp user data before sync";
                return;
            }
            
            size_t tmpUserUuidLen = uuid.size();
            size_t tmpUserImageDataLen = decryptedTmpUserDataLen - tmpUserUuidLen;
            unsigned char *tmpUserImageData = new unsigned char[tmpUserImageDataLen];
            std::copy(decryptedTmpUserData + tmpUserUuidLen, decryptedTmpUserData  + tmpUserUuidLen + tmpUserImageDataLen, tmpUserImageData);   
            
            QByteArray byteArray(reinterpret_cast<char*>(tmpUserImageData), tmpUserImageDataLen);
            
            tmpUser.type = operationType;
            tmpUser.id = uuid;
            tmpUser.username = username;
            tmpUser.userPwd = decryptedUserPwd;
            tmpUser.faceData = byteArray.toBase64();
            tmpUser.department = department;
            tmpUser.operationTime = operationTime;
            
            tmpUserList.append(tmpUser);
            
            if(decryptedTmpUserData != nullptr)
            {
                free(decryptedTmpUserData);
                decryptedTmpUserData = nullptr;
            }
            if(tmpUserImageData != nullptr)
            {
                delete[] tmpUserImageData;
                tmpUserImageData = nullptr;
            }
        }
        else if(tableName == "TemporaryUser" && operationType == "delete")
        {
            tmpUserInfo tmpUser;
            
            tmpUser.type = operationType;
            tmpUser.id = recordId;
            tmpUser.operationTime = operationTime;
            
            tmpUserList.append(tmpUser);
        }
        else if(tableName == "UsagePeriods" && (operationType == "add" || operationType == "update"))
        {
            usageTime time;
            
            QString uuid = recordId;
            QList<QString> list;
            QString userId;
            QString startTime;
            QString endTime;
            
            bool res = dbManager.queryUsagePeriodsData(recordId, list);
            if(!res || list.size() != 3)
            {
                continue;
            }
            
            userId = list[0];
            startTime = list[1];
            endTime = list[2];
            
            time.type = operationType;
            time.id = recordId;
            time.userId = userId;
            time.startTime = startTime;
            time.endTime = endTime;
            time.operationTime = operationTime;
            
            timeList.append(time);
        }
        else if(tableName == "UsagePeriods" && operationType == "delete")
        {
            usageTime time;
            
            time.type = operationType;
            time.id = recordId;
            time.operationTime = operationTime;
            
            timeList.append(time);
        }
    }
    
    QList<QList<QString>> loginLogList;
    dbManager.quertUnsyncLog(loginLogList);
    for(auto loginlog : loginLogList)
    {
        loginInfo info;
        info.username = loginlog[1];
        info.loginTime = loginlog[2];
        info.logoutTime = loginlog[3];
        info.department = loginlog[4];
        
        loginInfoList.append(info);
    }
    
    qDebug()<<"adminList.size "<<adminList.size();
    qDebug()<<"tmpUserList.size "<<tmpUserList.size();
    qDebug()<<"timeList.size "<<timeList.size();
    qDebug()<<"loginInfoList.size "<<loginInfoList.size();
    
    if(!adminList.isEmpty())
    {
//        for(auto item : adminList)
//        {
//            qDebug()<<item.id<<" "<<item.type<<" "<<item.username<<" "<<item.department<<" "<<item.operationTime;
//        }
        
        if(!sendAdminInfoWorkThread->isRunning())
        {
            sendAdminInfoWorkThread->start();
        }
        emit startSendAdminInfo(adminList);
    }
    
    if(!tmpUserList.isEmpty())
    {
        if(!sendTmpUserInfoWorkThread->isRunning())
        {
            sendTmpUserInfoWorkThread->start();
        }
        emit startSendTmpUserInfo(tmpUserList);
    }
    
    if(!timeList.isEmpty())
    {
        if(!sendUsageTimeInfoWorkThread->isRunning())
        {
            sendUsageTimeInfoWorkThread->start();
        }
        emit startSendUsageTimeInfo(timeList);
    }
    
    if(!loginInfoList.isEmpty())
    {
        if(!sendLogInfoWorkThread->isRunning())
        {
            sendLogInfoWorkThread->start();
        }
        emit startSendLogInfo(loginInfoList);
    }
}

bool syncWork::deleteUserFile(QString username)
{
    QString dirPath = QCoreApplication::applicationDirPath() + "/DetectDir/tempUser";
    QDir().mkpath(dirPath);  // 确保目录存在
    
    QString filename = dirPath + "/" + username + ".enc";
    
    QFile file(filename);
    if (!file.remove())
    {
        qDebug() << "删除文件" << filename << "失败:" << file.errorString();
        return false;
    }
    else
    {
        qDebug() << "删除文件成功";
        return true;
    }
}

void syncWork::onIniSyncWork()
{
//    qDebug() << "sync thread id: " << QThread::currentThreadId();
    
    timestampsWork = new getTimestampsWork();
    getTimestampsThread = new QThread();
    timestampsWork->moveToThread(getTimestampsThread);
    connect(getTimestampsThread, &QThread::finished, timestampsWork, &QObject::deleteLater);
    connect(this, &syncWork::startGetTimestamps, timestampsWork, &getTimestampsWork::startSending);
    connect(timestampsWork, &getTimestampsWork::calculateSuccess, this, &syncWork::receiveCalculateSuccess);
    connect(timestampsWork, &getTimestampsWork::workFinished, getTimestampsThread, &QThread::quit);
    
    adminInfoWork = new sendAdminInfoWork();
    sendAdminInfoWorkThread = new QThread();
    adminInfoWork->moveToThread(sendAdminInfoWorkThread);
    connect(sendAdminInfoWorkThread, &QThread::finished, adminInfoWork, &QObject::deleteLater);
    qRegisterMetaType<QList<adminInfo>>("QList<adminInfo>");
    connect(this, &syncWork::startSendAdminInfo, adminInfoWork, &sendAdminInfoWork::startSending);
    connect(adminInfoWork, &sendAdminInfoWork::sendAdminInfoSuccessfully, this, &syncWork::onSendAdminInfoSuccessfully);
    
    tmpUserInfoWork = new sendTmpUserInfoWork();
    sendTmpUserInfoWorkThread = new QThread();
    tmpUserInfoWork->moveToThread(sendTmpUserInfoWorkThread);
    connect(sendTmpUserInfoWorkThread, &QThread::finished, tmpUserInfoWork, &QObject::deleteLater);
    qRegisterMetaType<QList<tmpUserInfo>>("QList<tmpUserInfo>");
    connect(this, &syncWork::startSendTmpUserInfo, tmpUserInfoWork, &sendTmpUserInfoWork::startSending);
    connect(tmpUserInfoWork, &sendTmpUserInfoWork::sendTmpUserInfoSuccessfully, this, &syncWork::onSendTmpUserInfoSuccessfully);   

    usageTimeInfoWork = new sendTimeInfoWork();
    sendUsageTimeInfoWorkThread = new QThread();
    usageTimeInfoWork->moveToThread(sendUsageTimeInfoWorkThread);
    connect(sendUsageTimeInfoWorkThread, &QThread::finished, usageTimeInfoWork, &QObject::deleteLater);
    qRegisterMetaType<QList<usageTime>>("QList<usageTime>");
    connect(this, &syncWork::startSendUsageTimeInfo, usageTimeInfoWork, &sendTimeInfoWork::startSending);
    connect(usageTimeInfoWork, &sendTimeInfoWork::sendUsageTimeInfoSuccessfully, this, &syncWork::onSendUsageTimeInfoSuccessfully);   
    
    logInfoWork = new sendLogWork();
    sendLogInfoWorkThread = new QThread();
    logInfoWork->moveToThread(sendLogInfoWorkThread);
    connect(sendLogInfoWorkThread, &QThread::finished, logInfoWork, &QObject::deleteLater);
    qRegisterMetaType<QList<loginInfo>>("QList<loginInfo>");
    connect(this, &syncWork::startSendLogInfo, logInfoWork, &sendLogWork::startSending);    
    connect(logInfoWork, &sendLogWork::sendLogInfoSuccessfully, this, &syncWork::onSendLogInfoSuccessfully);       
}

void syncWork::onStartSyncWork()
{
//    qDebug() << "sync thread id: " << QThread::currentThreadId();
    
    if(firstSend)
    {
        if(!getTimestampsThread->isRunning())
        {
            getTimestampsThread->start();
        }
        emit startGetTimestamps(urlCommon + "/time");
    }
    else
    {
        sendSyncData();
    }  
}

void syncWork::receiveCalculateSuccess()
{
    firstSend = false;
    
    sendSyncData();
}

void syncWork::onSendAdminInfoSuccessfully(QList<adminInfo> serverAdminList, const QList<adminInfo> &adminList)
{
    databaseManager dbManager;
    dbManager.connectToDatabase();
    mySm4 sm4;
    
    //收到同步成功信号，先保存服务端发来的数据
    //先判断是否需要删除数据
    if(!serverAdminList.isEmpty())
    {
        QList<unsyncAdminInfo> unsyncAdminList;
        
        for(int i = 0; i < serverAdminList.size(); i++)
        {
            adminInfo serverAdmin = serverAdminList[i];
            if(serverAdmin.type == "delete")
            {
                dbManager.deleteAdminDataById(serverAdmin.id);
                dbManager.updateRecord(serverAdmin.id);
                serverAdminList.removeAt(i);
            }
        }
        
        QString uuid;
        QString username;
        QString userPwd;
        QString faceImagePath;
        QString department;
        
        dbManager.queryAdminData(uuid, username, userPwd, faceImagePath, department);
        
        for(const auto &serverAdmin : serverAdminList)
        {
            if(serverAdmin.type == "add" || serverAdmin.type == "update")
            {
                if(!uuid.isEmpty() && uuid != serverAdmin.id)
                {
                    unsyncAdminInfo unsyncAdmin;
                    unsyncAdmin.admin = serverAdmin;
                    unsyncAdmin.errorType = 2;
                    unsyncAdminList.append(unsyncAdmin);
                }
                else if(uuid.isEmpty())
                {
                    //先校验信息是否完整
                    if(serverAdmin.type.isEmpty() || serverAdmin.id.isEmpty() || serverAdmin.username.isEmpty() || 
                       serverAdmin.userPwd.isEmpty() || serverAdmin.faceData.isEmpty() || 
                       serverAdmin.department.isEmpty() || serverAdmin.operationTime.isEmpty())
                    {
                        qDebug()<<"待同步管理员信息不完整";
                        unsyncAdminInfo unsyncAdmin;
                        unsyncAdmin.admin = serverAdmin;
                        unsyncAdmin.errorType = 1;
                        unsyncAdminList.append(unsyncAdmin);
                        continue;
                    }
                    
                    QString dirPath = QCoreApplication::applicationDirPath() + "/DetectDir/tempUser";
                    QDir().mkpath(dirPath);  // 确保目录存在
                    QString savePath = dirPath + "/" + "administrator.enc";
                    
                    //加密用户人脸数据并保存
                    QString uuidString = serverAdmin.id;
                    QString faceDataStr = serverAdmin.faceData;
                    unsigned char *uuidData = reinterpret_cast<unsigned char*>
                            (const_cast<char*>(uuidString.toStdString().c_str()));
                    //    unsigned long uuidLen = sizeof(uuidData);
                    unsigned long uuidLen = uuidString.size();
                    
                    QByteArray faceByteArray = QByteArray::fromBase64(faceDataStr.toUtf8());
                    unsigned char *faceData = reinterpret_cast<unsigned char*>(faceByteArray.data());
                    unsigned long faceDataLen = faceByteArray.size();
                    unsigned char *result = new unsigned char[uuidLen + faceDataLen];
                    std::copy(uuidData, uuidData + uuidLen, result);
                    std::copy(faceData, faceData + faceDataLen, result + uuidLen);
                    
                    bool ret = sm4.encrypt(result, uuidLen + faceDataLen, savePath);
                    
                    if(result != nullptr)
                    {
                        delete[] result;
                        result = nullptr;
                    }  
                    faceData = nullptr; 
                    
                    if(!ret)
                    {
                        qDebug()<<"加密人脸数据失败";
                        unsyncAdminInfo unsyncAdmin;
                        unsyncAdmin.admin = serverAdmin;
                        unsyncAdmin.errorType = 3;
                        unsyncAdminList.append(unsyncAdmin);
                        continue;
                    }
                    
                    //加密密码
                    QString encryptedPwd;
                    bool ret2 = false;
                    if(!serverAdmin.userPwd.isEmpty())
                    {
                        QByteArray pwdBytes = serverAdmin.userPwd.toUtf8();
                        unsigned char *pwdData = reinterpret_cast<unsigned char*>(pwdBytes.data());
                        unsigned long pwdLen = pwdBytes.size();  // 不包含结尾的 '\0'
                        
                        ret2 = sm4.encryptStr(pwdData, pwdLen, encryptedPwd);
                    }
                    if(!ret2)
                    {
                        qDebug()<<"加密密码失败";
                        unsyncAdminInfo unsyncAdmin;
                        unsyncAdmin.admin = serverAdmin;
                        unsyncAdmin.errorType = 4;
                        unsyncAdminList.append(unsyncAdmin);
                        continue;
                    }
                    
                    //保存管理员数据
                    bool ret3 = dbManager.insertAdminData(serverAdmin.id, serverAdmin.username,
                                                          encryptedPwd, savePath, serverAdmin.department);
                    if(!ret3)
                    {
                        qDebug()<<"添加管理员数据失败";
                        unsyncAdminInfo unsyncAdmin;
                        unsyncAdmin.admin = serverAdmin;
                        unsyncAdmin.errorType = 5;
                        unsyncAdminList.append(unsyncAdmin);
                        continue;
                    }
                    
                    dbManager.updateRecord(serverAdmin.id);
                }
            }
        }
    }
    
    
    //将相应数据的状态修改为已同步    
    for(auto adminInfo : adminList)
    {
        QString recordId = adminInfo.id;

        bool res = dbManager.updateRecord(recordId);
        if(!res)
        {
            qDebug()<<"update SyncLog id failed: "<<recordId;
        }    
    }
}

void syncWork::onSendTmpUserInfoSuccessfully(const QList<tmpUserInfo> &serverTmpUserList, const QList<tmpUserInfo> &tmpUserList)
{
    databaseManager dbManager;
    dbManager.connectToDatabase();
    mySm4 sm4;
    
    //收到同步成功信号，先保存服务端发来的数据
    if(!serverTmpUserList.isEmpty())
    {
        QString dirPath = QCoreApplication::applicationDirPath() + "/DetectDir/tempUser";
        QDir().mkpath(dirPath);  // 确保目录存在
        
        QList<unsyncTmpUserInfo> unsyncTmpUserList;
        
        for(const auto &tmpUser : serverTmpUserList)
        {
            //先校验信息是否完整
            if(tmpUser.type.isEmpty() || tmpUser.id.isEmpty() || tmpUser.username.isEmpty() || 
               tmpUser.userPwd.isEmpty() || tmpUser.faceData.isEmpty() || 
               tmpUser.department.isEmpty() || tmpUser.operationTime.isEmpty())
            {
                unsyncTmpUserInfo unsyncTmpUser;
                unsyncTmpUser.tmpUser = tmpUser;
                unsyncTmpUser.errorType = 1;
                unsyncTmpUserList.append(unsyncTmpUser);
                continue;
            }
            
            //信息完整则开始录入
            if(tmpUser.type == "add" || tmpUser.type == "update")
            {
                QString savePath = dirPath + "/" + tmpUser.username + ".enc";
                QString uuidStr;
                
                //加密用户人脸数据并保存
                QString uuidString = tmpUser.id;
                QString faceDataStr = tmpUser.faceData;
                unsigned char *uuidData = reinterpret_cast<unsigned char*>
                        (const_cast<char*>(uuidString.toStdString().c_str()));
                //    unsigned long uuidLen = sizeof(uuidData);
                unsigned long uuidLen = uuidString.size();
                
                QByteArray faceByteArray = QByteArray::fromBase64(faceDataStr.toUtf8());
                unsigned char *faceData = reinterpret_cast<unsigned char*>(faceByteArray.data());
                unsigned long faceDataLen = faceByteArray.size();
                unsigned char *result = new unsigned char[uuidLen + faceDataLen];
                std::copy(uuidData, uuidData + uuidLen, result);
                std::copy(faceData, faceData + faceDataLen, result + uuidLen);
                
                bool ret = sm4.encrypt(result, uuidLen + faceDataLen, savePath);
                
                if(result != nullptr)
                {
                    delete[] result;
                    result = nullptr;
                }  
                faceData = nullptr; 
                
                if(!ret)
                {
                    qDebug()<<"加密人脸数据失败";
                    unsyncTmpUserInfo unsyncTmpUser;
                    unsyncTmpUser.tmpUser = tmpUser;
                    unsyncTmpUser.errorType = 3;
                    unsyncTmpUserList.append(unsyncTmpUser);
                    continue;
                }
                
                //加密密码
                QString encryptedPwd;
                bool ret2 = false;
                if(!tmpUser.userPwd.isEmpty())
                {
                    QByteArray pwdBytes = tmpUser.userPwd.toUtf8();
                    unsigned char *pwdData = reinterpret_cast<unsigned char*>(pwdBytes.data());
                    unsigned long pwdLen = pwdBytes.size();  // 不包含结尾的 '\0'
                    
                    ret2 = sm4.encryptStr(pwdData, pwdLen, encryptedPwd);
                }
                if(!ret2)
                {
                    qDebug()<<"加密密码失败";
                    unsyncTmpUserInfo unsyncTmpUser;
                    unsyncTmpUser.tmpUser = tmpUser;
                    unsyncTmpUser.errorType = 4;
                    unsyncTmpUserList.append(unsyncTmpUser);
                    continue;
                }
                
                bool res = dbManager.queryTempUserIdByName(tmpUser.username, uuidStr);
                if(!res && uuidStr != tmpUser.id)
                {
                    qDebug()<<"id不存在或临时用户名对应id不匹配";
                    //存储数据
                    bool ret3 = dbManager.insertTempUserData(tmpUser.id, tmpUser.username, 
                                                             encryptedPwd, savePath, 
                                                             tmpUser.department);
                    if(!ret3)
                    {
                        qDebug()<<"添加用户数据失败";
                        unsyncTmpUserInfo unsyncTmpUser;
                        unsyncTmpUser.tmpUser = tmpUser;
                        unsyncTmpUser.errorType = 5;
                        unsyncTmpUserList.append(unsyncTmpUser);
                        continue;
                    }
                    
                    dbManager.updateRecord(tmpUser.id);
                }
                else
                {
                    //更新数据
                    bool ret3 = dbManager.updateTempUserData(tmpUser.id, tmpUser.username, 
                                                             encryptedPwd, savePath, 
                                                             tmpUser.department);
                    if(!ret3)
                    {
                        qDebug()<<"更新用户数据失败";
                        unsyncTmpUserInfo unsyncTmpUser;
                        unsyncTmpUser.tmpUser = tmpUser;
                        unsyncTmpUser.errorType = 6;
                        unsyncTmpUserList.append(unsyncTmpUser);
                        continue;
                    }
                    
                    dbManager.updateRecord(tmpUser.id); 
                }                   
            }
            else if(tmpUser.type == "delete")
            {
                QString savePath = dirPath + "/" + tmpUser.username + ".enc";
                QString uuidStr;
                bool res = dbManager.queryTempUserIdByName(tmpUser.username, uuidStr);
                if(!res && uuidStr != tmpUser.id)
                {
                    qDebug()<<"id不存在或临时用户名对应id不匹配,无需删除";
                    continue;
                }
                
                bool res2 = dbManager.deleteTempUserDataById(tmpUser.id);
                if(!res2)
                {
                    qDebug()<<"删除用户数据失败";
                    unsyncTmpUserInfo unsyncTmpUser;
                    unsyncTmpUser.tmpUser = tmpUser;
                    unsyncTmpUser.errorType = 7;
                    unsyncTmpUserList.append(unsyncTmpUser);
                    continue;
                }
                
                deleteUserFile(tmpUser.username);
                
                dbManager.updateRecord(tmpUser.id);
            }
        }
    }
  
    //将相应数据的状态修改为已同步    
    for(auto tmpUserInfo : tmpUserList)
    {
        QString recordId = tmpUserInfo.id;

        bool res = dbManager.updateRecord(recordId);
        if(!res)
        {
            qDebug()<<"update SyncLog id failed: "<<recordId;
        }    
    }
}

void syncWork::onSendUsageTimeInfoSuccessfully(const QList<usageTime> &serverTimeList, const QList<usageTime> &usageTimeList)
{
    databaseManager dbManager;
    dbManager.connectToDatabase();
    mySm4 sm4;
    //收到同步成功信号，先保存服务端发来的数据
    QList<unsyncUsageTime> unsyncTimeList;
    
    for(const auto &time : serverTimeList)
    {
        //先校验信息是否完整
        if(time.type.isEmpty() || time.id.isEmpty() || time.userId.isEmpty() || 
           time.startTime.isEmpty() || time.endTime.isEmpty() || time.operationTime.isEmpty())
        {
            unsyncUsageTime unsyncServerTime;
            unsyncServerTime.time = time;
            unsyncServerTime.errorType = 1;
            unsyncTimeList.append(unsyncServerTime);
            continue;
        }
        //信息完整则开始录入
        if(time.type == "add" || time.type == "update")
        {
            QList<QString> periodsList;  
            dbManager.queryUsagePeriodsData(time.id, periodsList);
            
            if(!periodsList.isEmpty())
            {
                QString oldStartTime = periodsList[1];
                QString oldEndTime = periodsList[2];
                bool ret1 = dbManager.updateUsagePeriodData(time.id, oldStartTime, oldEndTime, time.startTime, time.endTime);
                if(!ret1)
                {
                    qDebug()<<"更新用户时间失败";
                    unsyncUsageTime unsyncServerTime;
                    unsyncServerTime.time = time;
                    unsyncServerTime.errorType = 2;
                    unsyncTimeList.append(unsyncServerTime);
                    continue;
                }
                dbManager.updateRecord(time.id);
            }
            else
            {
                bool ret1 = dbManager.insertUsagePeriodData(time.id, time.userId, time.startTime, time.endTime);
                if(!ret1)
                {
                    qDebug()<<"增加用户时间失败";
                    unsyncUsageTime unsyncServerTime;
                    unsyncServerTime.time = time;
                    unsyncServerTime.errorType = 3;
                    unsyncTimeList.append(unsyncServerTime);
                    continue;
                }
                dbManager.updateRecord(time.id);
            }
        }
        else if(time.type == "delete")
        {
            dbManager.deleteUsagePeriodData(time.userId, time.startTime, time.endTime);
            dbManager.updateRecord(time.id);
        }
    }
    
    //将相应数据的状态修改为已同步
    dbManager.connectToDatabase();
    
    for(auto timeInfo : usageTimeList)
    {
        QString recordId = timeInfo.id;

        bool res = dbManager.updateRecord(recordId);
        if(!res)
        {
            qDebug()<<"update SyncLog id failed: "<<recordId;
        }    
    }
}

void syncWork::onSendLogInfoSuccessfully(const QList<loginInfo> &logList)
{
    //收到同步成功信号，将相应数据的状态修改为已同步
    databaseManager dbManager;
    dbManager.connectToDatabase();
    
    for(auto logInfo : logList)
    {
//        QString recordId = logInfo.id;

//        bool res = dbManager.updateRecord(recordId);
//        if(!res)
//        {
//            qDebug()<<"update SyncLog id failed: "<<recordId;
//        }    
        QString username = logInfo.username;
        QString loginTime = logInfo.loginTime;
        QString logoutTime = logInfo.logoutTime;
        
        bool res = dbManager.updateLogSyncRecord(username, loginTime, logoutTime);
        if(!res)
        {
            qDebug()<<"update LoginLog failed: "<<username <<" "<<loginTime<<" " <<logoutTime;
        } 
    }
}
