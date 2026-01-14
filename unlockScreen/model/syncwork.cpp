#include "syncwork.h"
#include "globalVariables.h"
#include "model/databasemanager.h"
#include "model/mysm4.h"
#include "utf8.h"

#include <QDebug>
#include <QMetaType>
#include <QCoreApplication>
#include <QDir>
#include <QThread>
#include <QTimer>

// 共享变量（使用 std::atomic 以确保线程安全）
std::atomic<bool> getAdminInfoFinished(false);
std::atomic<bool> getTmpUserInfoFinished(false);
std::atomic<bool> getTimeInfoFinished(false);

syncWork::syncWork(QObject *parent) : QObject(parent)
{
    
}

void syncWork::getSyncData()
{
    if(!getAdminInfoWorkThread->isRunning())
    {
        getAdminInfoWorkThread->start();
    }
    if(!getTmpUserInfoWorkThread->isRunning())
    {
        getTmpUserInfoWorkThread->start();
    }
    if(!getUsageTimeInfoWorkThread->isRunning())
    {
        getUsageTimeInfoWorkThread->start();
    }
    emit startGetAdminInfo();
    emit startGetTmpUserInfo();
    emit startGetUsageTimeInfo();
    
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        if(getAdminInfoFinished && getTmpUserInfoFinished && getTimeInfoFinished)
        {
            emit syncFinished();
        }
    });
    timer->start(100); // 每 100ms 检查一次    
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
    connect(timestampsWork, &getTimestampsWork::getTimeStampsFailed, this, &syncWork::onGetTimeStampsFailed);
    
    adminInfoWork = new getAdminInfoWork();
    getAdminInfoWorkThread = new QThread();
    adminInfoWork->moveToThread(getAdminInfoWorkThread);
    connect(getAdminInfoWorkThread, &QThread::finished, adminInfoWork, &QObject::deleteLater);
    connect(this, &syncWork::startGetAdminInfo, adminInfoWork, &getAdminInfoWork::startSending);
    qRegisterMetaType<QList<adminInfo>>("QList<adminInfo>");
    connect(adminInfoWork, &getAdminInfoWork::getAdminInfoSuccessfully, this, &syncWork::onGetAdminInfoSuccessfully);
    
    tmpUserInfoWork = new getTmpUserInfoWork();
    getTmpUserInfoWorkThread = new QThread();
    tmpUserInfoWork->moveToThread(getTmpUserInfoWorkThread);
    connect(getTmpUserInfoWorkThread, &QThread::finished, tmpUserInfoWork, &QObject::deleteLater);    
    connect(this, &syncWork::startGetTmpUserInfo, tmpUserInfoWork, &getTmpUserInfoWork::startSending);
    qRegisterMetaType<QList<tmpUserInfo>>("QList<tmpUserInfo>");
    connect(tmpUserInfoWork, &getTmpUserInfoWork::getTmpUserInfoSuccessfully, this, &syncWork::onGetTmpUserInfoSuccessfully);   

    usageTimeInfoWork = new getTimeInfoWork();
    getUsageTimeInfoWorkThread = new QThread();
    usageTimeInfoWork->moveToThread(getUsageTimeInfoWorkThread);
    connect(getUsageTimeInfoWorkThread, &QThread::finished, usageTimeInfoWork, &QObject::deleteLater);
    connect(this, &syncWork::startGetUsageTimeInfo, usageTimeInfoWork, &getTimeInfoWork::startSending);
    qRegisterMetaType<QList<usageTime>>("QList<usageTime>");
    connect(usageTimeInfoWork, &getTimeInfoWork::getUsageTimeInfoSuccessfully, this, &syncWork::onGetUsageTimeInfoSuccessfully);   
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
        getSyncData();
    }  
}

void syncWork::receiveCalculateSuccess()
{
    firstSend = false;
    
    getSyncData();
}

void syncWork::onGetAdminInfoSuccessfully(QList<adminInfo> serverAdminList)
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
    
    getAdminInfoFinished = true;
}

void syncWork::onGetTmpUserInfoSuccessfully(const QList<tmpUserInfo> &serverTmpUserList)
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
    
    getTmpUserInfoFinished = true;
}

void syncWork::onGetUsageTimeInfoSuccessfully(const QList<usageTime> &serverTimeList)
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
    
    getTimeInfoFinished = true;
}

void syncWork::onGetTimeStampsFailed()
{
    emit syncFinished();
}
