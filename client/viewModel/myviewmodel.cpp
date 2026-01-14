#include "myviewmodel.h"
#include "utf8.h"

#include <QImage>
#include <QDebug>
#include <QDir>
#include <QUuid>
#include <QCoreApplication>
#include <QMessageBox>
#include <QThread>
#include <QMetaType>

myViewModel::myViewModel(QObject *parent) : QObject(parent)
{
    pSm4 = new mySm4();
    
    pCamera = new DvtCamera();

    connect(pCamera, &DvtCamera::sendFaceData, this, &myViewModel::onSendFaceData);
    connect(pCamera, &DvtCamera::sendCamMessage, this, &myViewModel::onSendCamMessage);    
    
    pCamIniWork = new camIniWork();
    camIniThread = new QThread();
    
    pCamIniWork->moveToThread(camIniThread);
    
    connect(camIniThread, &QThread::finished, pCamIniWork, &camIniWork::deleteLater);
    qRegisterMetaType<DvtCamera*>("DvtCamera*");
    connect(this, &myViewModel::startIniCam, pCamIniWork, &camIniWork::onStartIniCam);
}

myViewModel::~myViewModel()
{
    if(!pSm4)
    {
        delete pSm4;
        pSm4 = nullptr;
    }
    
    if(!pCamera)
    {
        delete pCamera;
        pCamera = nullptr;
    }
}

void myViewModel::iniClientConfig()
{
    bool res = dbManager.insertConfigData(false, "");
    if(!res)
    {
        qDebug()<<"初始化客户端配置表数据失败";
    }
}

bool myViewModel::isFirstLogin()
{
    QString id;
    QString username;
    QString userPwd;
    QString path;
    QString department;
    
    dbManager.queryAdminData(id, username, userPwd, path, department);
    if(id.isEmpty() || username.isEmpty() || userPwd.isEmpty() || path.isEmpty() || department.isEmpty())
    {
        return true;
    }
    else
    {
        return false;
    }
}

void myViewModel::checkLoginLog()
{
    dbManager.cleanOldLoginLogs(365);
}

bool myViewModel::isPwdChecked()
{
    QString username;
    QString pwd;
    
    dbManager.querySysData(username, pwd);
    
    if(username.isEmpty() || pwd.isEmpty())
    {
        dbManager.deleteSysData();
        return false;
    }
    else
    {
        return true;
    }
}

void myViewModel::dataToPixmap(unsigned char* faceData, unsigned long faceDataLen)
{
    QByteArray byteArray(reinterpret_cast<char*>(faceData), faceDataLen);
    
    QImage image = QImage::fromData(byteArray);
    if (image.isNull()) 
    {
        // 处理图像加载失败的情况
        qWarning() << "Failed to load image from data!";
        emit sendVmMessage("处理人脸图片失败");
        return;
    }
    
    emit sendVmMessage("处理人脸图片成功");
    QPixmap pixmap = QPixmap::fromImage(image);
    emit sendPixmap(pixmap);
    
}

QPixmap myViewModel::getPixmap(unsigned char *faceData, unsigned long faceDataLen)
{
    QByteArray byteArray(reinterpret_cast<char*>(faceData), faceDataLen);
    QPixmap pixmap;
    QImage image = QImage::fromData(byteArray);
    if (image.isNull()) 
    {
        // 处理图像加载失败的情况
        qWarning() << "Failed to load image from data!";
        return pixmap;
    }
    
    pixmap = QPixmap::fromImage(image);
    return pixmap;
}

bool myViewModel::writeEncryptedDataToFile(const uint8_t *data, size_t len, const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "Cannot open file for writing!";
        return false;
    }
    file.write(reinterpret_cast<const char*>(data), len);
    file.close();
    return true;
}

bool myViewModel::addTmpUserInfo(const bool &tmpUserExist, const QString &uuid, const QString &username, const QString &userPwd, const QString &faceImagePath, const QString &startTime, const QString &endTime, const QString &department)
{
    QUuid timmeUuid = QUuid::createUuid();
    QString timeUuidString = timmeUuid.toString(QUuid::WithoutBraces);
    timeUuidString.remove("-");
    
    if(tmpUserExist)
    {
        bool res = dbManager.insertUsagePeriodData(timeUuidString, uuid, startTime, endTime);
        return res;
    }
    else
    {
        bool res1 = dbManager.insertTempUserData(uuid, username, userPwd, faceImagePath, department);
        if(!res1)
        {
            return res1;
        }
        
        bool res2 = dbManager.insertUsagePeriodData(timeUuidString, uuid, startTime, endTime);
        return res2;
    }
}

bool myViewModel::addTmpUserInfo(const QString &uuid, const QString &username, const QString &userPwd, const QString &faceImagePath, const QString &department)
{
    bool res = dbManager.insertTempUserData(uuid, username, userPwd, faceImagePath, department);
    return res;
}

bool myViewModel::checkSysUserAndPwd(QString username, QString pwd)
{
    pwdCheck checkItem;
    bool res = checkItem.ValidatePassword(username.toStdWString(), pwd.toStdWString());
    return res;
}

bool myViewModel::saveSysUserAndPwd(QString username, QString pwd)
{
    //因为credentialProvider程序中使用的是宽字符，因此用户名与密码均也按照宽字符加密
    QByteArray usernameBytes;
    usernameBytes.reserve(username.length() * 2); // UTF-16 每个字符占 2 字节
    const ushort* utf16Data = username.utf16();   // 获取 UTF-16 数据指针
    for (int i = 0; i < username.length(); ++i)
    {
        // 显式按 Little-Endian 写入字节
        usernameBytes.append(static_cast<char>(utf16Data[i] & 0xFF));      // 低位字节
        usernameBytes.append(static_cast<char>((utf16Data[i] >> 8) & 0xFF)); // 高位字节
    }
    unsigned char *usernameData = reinterpret_cast<unsigned char*>(usernameBytes.data());
    unsigned long usernameLen = usernameBytes.size();
//    unsigned char *usernameData = reinterpret_cast<unsigned char*>
//            (const_cast<char*>(username.toStdString().c_str()));
//    unsigned long usernameLen = username.size();
    QString strUsername;
    pSm4->encryptStr(usernameData, usernameLen, strUsername);
    
    
    QByteArray pwdBytes;
    pwdBytes.reserve(pwd.length() * 2);
    const ushort* pwdUtf16Data = pwd.utf16();
    for (int i = 0; i < pwd.length(); ++i)
    {
        pwdBytes.append(static_cast<char>(pwdUtf16Data[i] & 0xFF));
        pwdBytes.append(static_cast<char>((pwdUtf16Data[i] >> 8) & 0xFF));
    }
    unsigned char *pwdData = reinterpret_cast<unsigned char*>(pwdBytes.data());
    unsigned long pwdLen = pwdBytes.size();
//    unsigned char *pwdData = reinterpret_cast<unsigned char*>
//            (const_cast<char*>(pwd.toStdString().c_str()));
//    unsigned long pwdLen = pwd.size();
    QString strPwd;
    pSm4->encryptStr(pwdData, pwdLen, strPwd);
    
    qDebug()<<strUsername;
    qDebug()<<strPwd;
    
    if(strUsername.isEmpty() || strPwd.isEmpty())
    {
        return false;
    }
    
    //先查询当前表中是否已存在用户名和密码
    QString user;
    QString pwd2;
    dbManager.querySysData(user, pwd2);
    if(!user.isEmpty() || !pwd2.isEmpty())
    {
        dbManager.deleteSysData();
    }
    
    bool res = dbManager.insertSysData(strUsername, strPwd);
    return res;
}

bool myViewModel::updateTmpUserInfo(const QString &uuid, const QString &username, const QString &userPwd, const QString &faceImagePath, const QString &department)
{
    bool res = dbManager.updateTempUserData(uuid, username, userPwd, faceImagePath, department);
    return res;
}

QString myViewModel::getLastLoginIdentity()
{
    return dbManager.queryLoginIdentity();
}

void myViewModel::onIniCheckThread()
{
    screenStatusCheckWork = new screenStatusCheck();
    checkThread = new QThread();
    
    screenStatusCheckWork->moveToThread(checkThread);
    
    connect(checkThread, &QThread::finished, screenStatusCheckWork, &screenStatusCheck::deleteLater);
    connect(this, &myViewModel::checkStart, screenStatusCheckWork, &screenStatusCheck::onCheckStart);
    connect(screenStatusCheckWork, &screenStatusCheck::logout, this, &myViewModel::onLogout);
    checkThread->start();
    
    emit checkStart();
}

void myViewModel::onIniSyncThread()
{
    pSyncWork = new syncWork();
    syncThread = new QThread();
    
    pSyncWork->moveToThread(syncThread);
    
    connect(syncThread, &QThread::finished, pSyncWork, &syncWork::deleteLater);
    connect(this, &myViewModel::iniSyncWork, pSyncWork, &syncWork::onIniSyncWork);
    connect(this, &myViewModel::startSyncWork, pSyncWork, &syncWork::onStartSyncWork);
    
    qDebug() << "main thread id: " << QThread::currentThreadId();
    
    syncThread->start();
    
    emit iniSyncWork();
    emit startSyncWork();
}

void myViewModel::onIniDatabase()
{
    dbManager.connectToDatabase();
    dbManager.createTables();
    
    iniClientConfig();
    checkLoginLog();
}

void myViewModel::onIniCam()
{    
    if(!camIniThread->isRunning())
    {
        camIniThread->start();
    }
    emit startIniCam(pCamera);
    
    //    pCamera->iniCamera();
}

void myViewModel::onUniniCam()
{
    pCamera->closeCamera();
    pCamera->uniniCamera();
}

void myViewModel::onOpenCam(HWND m_hWnd, int nVideoWindowWidth, int nVideoWindowHeight)
{
    pCamera->openCamera(m_hWnd, 0, 0, nVideoWindowWidth, nVideoWindowHeight);
}

void myViewModel::onStartDetection()
{
    pCamera->startFaceDetaction();
}

void myViewModel::onSendFaceData(unsigned char *faceData, unsigned long faceDataLen)
{
    curFaceData = faceData;
    curFaceDataLen = faceDataLen;
//    pCamera->closeCamera();
//    pCamera->uniniCamera();
    
    dataToPixmap(curFaceData, curFaceDataLen);
}

void myViewModel::onSendCamMessage(QString message)
{
    emit repostCamMessage(message);
}

void myViewModel::onUserRegistration(QString user, QString pwd, QString department)
{
    //先关闭相机
    pCamera->closeCamera();
    pCamera->uniniCamera();
    
    QString dirPath = QCoreApplication::applicationDirPath() + "/DetectDir/admin";
    QDir().mkpath(dirPath);  // 确保目录存在
    
    QString savePath = dirPath + "/administrator.enc";
    
    //生成uuid
    QUuid newUuid = QUuid::createUuid();
    QString uuidString = newUuid.toString(QUuid::WithoutBraces);
    uuidString.remove("-"); 
    
    unsigned char *uuidData = reinterpret_cast<unsigned char*>
            (const_cast<char*>(uuidString.toStdString().c_str()));
//    unsigned long uuidLen = sizeof(uuidData);
    unsigned long uuidLen = uuidString.size();
    
    unsigned char *result = new unsigned char[uuidLen + curFaceDataLen];
    std::copy(uuidData, uuidData + uuidLen, result);
    std::copy(curFaceData, curFaceData + curFaceDataLen, result + uuidLen);
    
    bool ret1 = pSm4->encrypt(result, uuidLen + curFaceDataLen, savePath);
    
    
    QString encryptedPwd;
    bool ret2 = false;
    if(!pwd.isEmpty())
    {
        QByteArray pwdBytes = pwd.toUtf8();
        unsigned char *pwdData = reinterpret_cast<unsigned char*>(pwdBytes.data());
        unsigned long pwdLen = pwdBytes.size();  // 不包含结尾的 '\0'
        
        ret2 = pSm4->encryptStr(pwdData, pwdLen, encryptedPwd);
    }
    
    if(ret1 && ret2)
    {
        delete[] result;
        
        bool ret3 = dbManager.insertAdminData(uuidString, user, encryptedPwd, savePath, department);
        pCamera->freeData(curFaceData);
        curFaceData = nullptr;
        
        emit sendRegisterResult(ret3);
    }
}

void myViewModel::onUserReregistration(QString user, QString pwd, QString department)
{
    //先关闭相机
    pCamera->closeCamera();
    pCamera->uniniCamera();
    
    if(pwd.isEmpty())
    {
        emit sendRegisterResult(false);
        return;
    }
    
    QString dirPath = QDir::currentPath() + "/DetectDir/admin";
    QDir().mkpath(dirPath);  // 确保目录存在
    
    QString savePath = dirPath + "/administrator.enc";
    
    //生成uuid
    QUuid newUuid = QUuid::createUuid();
    QString uuidString = newUuid.toString(QUuid::WithoutBraces);
    uuidString.remove("-"); 
    
    unsigned char *uuidData = reinterpret_cast<unsigned char*>
            (const_cast<char*>(uuidString.toStdString().c_str()));
//    unsigned long uuidLen = sizeof(uuidData);
    unsigned long uuidLen = uuidString.size();
    
    unsigned char *result = new unsigned char[uuidLen + curFaceDataLen];
    std::copy(uuidData, uuidData + uuidLen, result);
    std::copy(curFaceData, curFaceData + curFaceDataLen, result + uuidLen);
    
    bool ret = pSm4->encrypt(result, uuidLen + curFaceDataLen, savePath);
    
    if(ret)
    {
        delete[] result;
        
        QString encryptedPwd;
        QByteArray pwdBytes = pwd.toUtf8();
        unsigned char *pwdData = reinterpret_cast<unsigned char*>(pwdBytes.data());
        unsigned long pwdLen = pwdBytes.size();  // 不包含结尾的 '\0'
        
        bool ret2 = pSm4->encryptStr(pwdData, pwdLen, encryptedPwd);
        if(!ret2)
        {
            emit sendRegisterResult(ret2);
            return;
        }
        
        bool ret3 = dbManager.deleteAdminData();
        if(!ret3)
        {
            emit sendRegisterResult(ret3);
            return;
        }
        
        bool ret4 = dbManager.insertAdminData(uuidString, user, encryptedPwd, savePath, department);
        pCamera->freeData(curFaceData);
        curFaceData = nullptr;
        
        emit sendRegisterResult(ret4);
    }
}

void myViewModel::onCheckUserAndPwd(QString username, QString pwd)
{
    bool res = checkSysUserAndPwd(username, pwd);
    if(res)
    {
        bool res2 = saveSysUserAndPwd(username, pwd);
        emit sendCheckResult(res2);
    }
    else
    {
        emit sendCheckResult(false);
    }
}

void myViewModel::onCheckUserAndPwdOnly(QString username, QString pwd)
{
    bool res = checkSysUserAndPwd(username, pwd);
    emit sendCheckOnlyResult(res);
}

void myViewModel::onSaveUserAndPwd(QString username, QString pwd)
{
    bool res = saveSysUserAndPwd(username, pwd);
    emit sendCheckResult(res);
}

//void myViewModel::onCheckUserAndPwd(QString username, QString pwd)
//{
//    pwdCheck checkItem;
    
//    bool res = checkItem.ValidatePassword(username.toStdWString(), pwd.toStdWString());
//    if(res)
//    {
//        unsigned char *usernameData = reinterpret_cast<unsigned char*>
//                (const_cast<char*>(username.toStdString().c_str()));
//        unsigned long usernameLen = username.size();
//        QString strUsername;
//        pSm4->encryptStr(usernameData, usernameLen, strUsername);
        
//        unsigned char *pwdData = reinterpret_cast<unsigned char*>
//                (const_cast<char*>(pwd.toStdString().c_str()));
//        unsigned long pwdLen = pwd.size();
//        QString strPwd;
//        pSm4->encryptStr(pwdData, pwdLen, strPwd);
        
//        qDebug()<<strUsername;
//        qDebug()<<strPwd;
        
//        if(strUsername.isEmpty() || strPwd.isEmpty())
//        {
//            emit sendCheckResult(false);
//        }
        
//        //先查询当前表中是否已存在用户名和密码
//        QString user;
//        QString pwd;
//        dbManager.querySysData(user, pwd);
//        if(!user.isEmpty() || !pwd.isEmpty())
//        {
//            dbManager.deleteSysData();
//        }
        
//        bool res2 = dbManager.insertSysData(strUsername, strPwd);
//        emit sendCheckResult(res2);
//    }
//    else
//    {
//        emit sendCheckResult(res);
//    }   
//}

void myViewModel::onCheckUsername()
{
    QString uuid;
    QString username;
    QString userPwd;
    QString path;
    QString department;
    
    dbManager.queryAdminData(uuid, username, userPwd, path, department);
    
    emit refreshUsername(username);
}

void myViewModel::onGetAllTmpUserData()
{
    QList<QPair<QString, QPair<QString, QString> > > dataList = 
                                            dbManager.getAllTemporaryUsersWithUsagePeriods();
    
    emit sendAllTmpUserData(dataList);
}

void myViewModel::onGetTempUserLogin()
{
    bool tempUserLogin = dbManager.queryTempUserLogin();
    emit sendTmpUserLogin(tempUserLogin);
}

void myViewModel::onSetTempUserLogin(bool tmpUserLogin)
{
    dbManager.updateTempUserLogin(tmpUserLogin);
}

void myViewModel::onDeleteTmpUserData(int index, QString tmpUserName, QString startTime, QString endTime)
{
    QString tmpUserId;
    bool res = dbManager.queryTempUserIdByName(tmpUserName, tmpUserId);
    if(!res || tmpUserId.isEmpty())
    {
        emit sendDelTmpUserDataResult(false, index);
    }
    else
    {
        bool res2 = dbManager.deleteUsagePeriodData(tmpUserId, startTime, endTime);
        emit sendDelTmpUserDataResult(res2, index);
    }
}

void myViewModel::onChangeTmpUserPeriod(const QString &tmpUserName, const QString &startTime, const QString &endTime)
{
    QString tmpUserId;
    bool res = dbManager.queryTempUserIdByName(tmpUserName, tmpUserId);
    if(res && !tmpUserId.isEmpty())
    {
        QString tmpUserImagePath;
        bool res = dbManager.queryTempUserImgPathById(tmpUserId, tmpUserImagePath);
        if(res && !tmpUserImagePath.isEmpty())
        {
            uint8_t *encryptedTmpUserData = nullptr;
            size_t encryptedTmpUserDataLen = 0;
            
            uint8_t *decryptedTmpUserData = nullptr;
            size_t decryptedTmpUserDataLen = 0;
            
            if(!pSm4->decrypt(tmpUserImagePath, encryptedTmpUserData, encryptedTmpUserDataLen,
                              decryptedTmpUserData, decryptedTmpUserDataLen))
            {
                qDebug()<<"Failed to decrypt temp user data";
                return;
            }
            //剔除uuid
            size_t tmpUserUuidLen = tmpUserId.size();
            size_t tmpUserImageDataLen = decryptedTmpUserDataLen - tmpUserUuidLen;
            unsigned char *tmpUserImageData = new unsigned char[tmpUserImageDataLen];
            std::copy(decryptedTmpUserData + tmpUserUuidLen, decryptedTmpUserData  + tmpUserUuidLen + tmpUserImageDataLen, tmpUserImageData);   
            //只保留解密后的图片数据
            if(encryptedTmpUserData != nullptr)
            {
                free(encryptedTmpUserData);
                encryptedTmpUserData = nullptr;
            }
            if(decryptedTmpUserData != nullptr)
            {
                free(decryptedTmpUserData);
                decryptedTmpUserData = nullptr;
            }
            
            QPixmap tmpUserPixmap = getPixmap(tmpUserImageData, tmpUserImageDataLen);
            
            //转换为图片后删除原始数据
            if(tmpUserImageData != nullptr)
            {
                free(tmpUserImageData);
                tmpUserImageData = nullptr;
            }
            
            emit changeTmpUserPeriod2(tmpUserId, tmpUserName, tmpUserPixmap, startTime, endTime);
        }
        
    }
}

void myViewModel::onDeleteTmpUserData2(int index, const QString &uuidStr, const QString &startTime, const QString &endTime)
{
    bool res = dbManager.deleteUsagePeriodData(uuidStr, startTime, endTime);
    emit sendDelTmpUserDataResult2(res, index);
}

void myViewModel::onTmpUserRegistration(QString username, QString userPwd, QString startTime, QString endTime, QString department)
{
    QString dirPath = QCoreApplication::applicationDirPath() + "/DetectDir/tempUser";
    QDir().mkpath(dirPath);  // 确保目录存在
    
    if(userPwd.isEmpty())
    {
        qDebug()<<"userPwd is empty";
        sendTmpRegisterResult(false, "");
        return;
    }
    
    QString savePath = dirPath + "/" + username + ".enc";
    
    bool tmpUserExist = true;
    
    //先判断该人脸是否已经注册了管理员
    //获取管理员信息
    QString adminUuid;
    QString adminUsername;
    QString adminUserPwd;
    QString adminFaceImagePath;
    QString adminDepartment;
    dbManager.queryAdminData(adminUuid, adminUsername, adminUserPwd, adminFaceImagePath, adminDepartment);
    
    //解密管理员数据
    uint8_t *encryptedAdminData = nullptr;
    size_t encryptedAdminDataLen = 0;
    
    uint8_t *decryptedAdminData = nullptr;
    size_t decrypted_len = 0;
    
    if(!pSm4->decrypt(adminFaceImagePath, encryptedAdminData, encryptedAdminDataLen,
                      decryptedAdminData, decrypted_len))
    {
        qDebug()<<"Failed to decrypt admin data";
        sendTmpRegisterResult(false, "");
        return;
    }
    
    //剔除uuid
    size_t adminUuidLen = adminUuid.size();
    size_t adminImageDataLen = decrypted_len - adminUuidLen;
    unsigned char *adminImageData = new unsigned char[adminImageDataLen];
    std::copy(decryptedAdminData + adminUuidLen, decryptedAdminData  + adminUuidLen + adminImageDataLen, adminImageData);   
    
//    QString tmpFileName ="D:/Qt5.12.9Project/build-faceRecognition-Desktop_Qt_5_12_9_MSVC2017_64bit-Release/lnsoft-otfr/DetectDir/image.jpg";
//    QFile file(tmpFileName);
//    if (!file.open(QIODevice::WriteOnly))
//    {
//        qDebug() << "Cannot open file for writing!";
//        return ;
//    }

//    file.write(reinterpret_cast<const char*>(adminImageData), adminImageDataLen);
//    file.close();
    
    //比对临时用户和管理员人脸相似度
    float fSimilarity;
    pCamera->facialComparision(adminImageData, adminImageDataLen, curFaceData, curFaceDataLen, &fSimilarity);
    if(fSimilarity >= similarityThreshold)
    {
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
        pCamera->freeData(curFaceData);
        curFaceData = nullptr;
        //关闭相机
        pCamera->closeCamera();
//        pCamera->uniniCamera();
        emit sendTmpRegisterResult(false, "该用户已注册管理员权限");
        return;
    }
    
    
    //然后判断临时用户名是否已经存在，存在则获取uuid，没有则先对比人脸相似度，然后生成新的uuid
    QString uuidString;
    dbManager.queryTempUserIdByName(username, uuidString);
    if(uuidString.isEmpty())
    {
        //不存在就先对比已注册临时用户人脸相似度
        QList<QList<QString>> tmpUsersData;
        dbManager.queryTempUserData(tmpUsersData);
        
        for(const QList<QString> &tmpUser : tmpUsersData)
        {
            QString tmpUserUuid = tmpUser[0];
            QString tmpUserImagePath = tmpUser[3];
            
            uint8_t *encryptedTmpUserData = nullptr;
            size_t encryptedTmpUserDataLen = 0;
            
            uint8_t *decryptedTmpUserData = nullptr;
            size_t decryptedTmpUserDataLen = 0;
            
            if(!pSm4->decrypt(tmpUserImagePath, encryptedTmpUserData, encryptedTmpUserDataLen,
                              decryptedTmpUserData, decryptedTmpUserDataLen))
            {
                qDebug()<<"Failed to decrypt temp user data";
                emit sendTmpRegisterResult(false, "解密临时授权用户人脸数据出错");
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
            
            pCamera->facialComparision(tmpUserImageData, tmpUserImageDataLen, curFaceData, curFaceDataLen, &fSimilarity);
            if(tmpUserImageData != nullptr)
            {
                delete[] tmpUserImageData;
                tmpUserImageData = nullptr;
            }
            
            if(fSimilarity >= similarityThreshold)
            {
                pCamera->freeData(curFaceData);
                curFaceData = nullptr;
                //关闭相机
                pCamera->closeCamera();
//                pCamera->uniniCamera();
                emit sendTmpRegisterResult(false, "该用户已注册临时用户");
                return;
            }
        }
        
        //与已注册临时用户人脸信息不同则生成新的uuid
        QUuid newUuid = QUuid::createUuid();
        uuidString = newUuid.toString(QUuid::WithoutBraces);
        uuidString.remove("-");
        
        tmpUserExist = false;
    }
     
    
    unsigned char *uuidData = reinterpret_cast<unsigned char*>
            (const_cast<char*>(uuidString.toStdString().c_str()));
//    unsigned long uuidLen = sizeof(uuidData);
    unsigned long uuidLen = uuidString.size();
    
    unsigned char *result = new unsigned char[uuidLen + curFaceDataLen];
    std::copy(uuidData, uuidData + uuidLen, result);
    std::copy(curFaceData, curFaceData + curFaceDataLen, result + uuidLen);
    
    bool ret = pSm4->encrypt(result, uuidLen + curFaceDataLen, savePath);
    
    if(result != nullptr)
    {
        delete[] result;
        result = nullptr;
    }  
    pCamera->freeData(curFaceData);
    curFaceData = nullptr;   
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
    //关闭相机
    pCamera->closeCamera();
//    pCamera->uniniCamera();
    
    QString encryptedPwd;
    QByteArray pwdBytes = userPwd.toUtf8();
    unsigned char *pwdData = reinterpret_cast<unsigned char*>(pwdBytes.data());
    unsigned long pwdLen = pwdBytes.size();  // 不包含结尾的 '\0'
    
    bool ret2 = pSm4->encryptStr(pwdData, pwdLen, encryptedPwd);
    
    if(ret && ret2)
    { 
        bool ret2 = addTmpUserInfo(tmpUserExist, uuidString, username, encryptedPwd, savePath, startTime, endTime, department);       
        emit sendTmpRegisterResult(ret2, "");
    }
    else
    {
        emit sendTmpRegisterResult(false, "");
    }
}

void myViewModel::onTmpUserRegistration2(QString tmpUsername, QString userPwd, QString department)
{
    QString dirPath = QCoreApplication::applicationDirPath() + "/DetectDir/tempUser";
    QDir().mkpath(dirPath);  // 确保目录存在
    
    if(userPwd.isEmpty())
    {
        qDebug()<<"userPwd is empty";
        sendTmpRegisterResult2(false, "");
        return;
    }
    
    QString savePath = dirPath + "/" + tmpUsername + ".enc";
    
    bool tmpUserExist = true;
    
    //先判断该人脸是否已经注册了管理员
    //获取管理员信息
    QString adminUuid;
    QString adminUsername;
    QString adminUserPwd;
    QString adminFaceImagePath;
    QString adminDepartment;
    dbManager.queryAdminData(adminUuid, adminUsername, adminUserPwd, adminFaceImagePath,adminDepartment);
    
    //解密管理员数据
    uint8_t *encryptedAdminData = nullptr;
    size_t encryptedAdminDataLen = 0;
    
    uint8_t *decryptedAdminData = nullptr;
    size_t decrypted_len = 0;
    
    if(!pSm4->decrypt(adminFaceImagePath, encryptedAdminData, encryptedAdminDataLen,
                      decryptedAdminData, decrypted_len))
    {
        qDebug()<<"Failed to decrypt admin data";
        sendTmpRegisterResult2(false, "解密管理员人脸数据出错");
        return;
    }
    
    //剔除uuid
    size_t adminUuidLen = adminUuid.size();
    size_t adminImageDataLen = decrypted_len - adminUuidLen;
    unsigned char *adminImageData = new unsigned char[adminImageDataLen];
    std::copy(decryptedAdminData + adminUuidLen, decryptedAdminData  + adminUuidLen + adminImageDataLen, adminImageData);   
    
    //比对临时用户和管理员人脸相似度
    float fSimilarity;
    pCamera->facialComparision(adminImageData, adminImageDataLen, curFaceData, curFaceDataLen, &fSimilarity);
    if(fSimilarity >= similarityThreshold)
    {
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
        pCamera->freeData(curFaceData);
        curFaceData = nullptr;
        //关闭相机
        pCamera->closeCamera();
        pCamera->uniniCamera();
        emit sendTmpRegisterResult2(false, "该用户已注册管理员权限");
        return;
    }
    
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
    
    //然后判断临时用户名是否已经存在，存在则询问用户是否更新用户人脸，否则退出
    QString uuidString;
    dbManager.queryTempUserIdByName(tmpUsername, uuidString);
    if(!uuidString.isEmpty())
    {
        QMessageBox:: StandardButton result = QMessageBox::information
                (NULL, "用户已存在", "该用户已注册，是否变更人脸信息？", QMessageBox::Yes|QMessageBox::No);
        switch (result)
        {
        case QMessageBox::Yes:
        {
            unsigned char *uuidData = reinterpret_cast<unsigned char*>
                    (const_cast<char*>(uuidString.toStdString().c_str()));
        //    unsigned long uuidLen = sizeof(uuidData);
            unsigned long uuidLen = uuidString.size();
            
            unsigned char *result = new unsigned char[uuidLen + curFaceDataLen];
            std::copy(uuidData, uuidData + uuidLen, result);
            std::copy(curFaceData, curFaceData + curFaceDataLen, result + uuidLen);
            
            bool ret = pSm4->encrypt(result, uuidLen + curFaceDataLen, savePath);
            
            if(result != nullptr)
            {
                delete[] result;
                result = nullptr;
            }  
            pCamera->freeData(curFaceData);
            curFaceData = nullptr;   
            
            //关闭相机
            pCamera->closeCamera();
            pCamera->uniniCamera();
            
            if(ret)
            {
                emit sendTmpRegisterResult2(ret, "");
            }
            
            break;
        }
        case QMessageBox::No:
            return;
        default:
            break;
        }
    }
    else
    {
        //用户名不存在，先和已注册临时用户人脸信息进行比对
        QList<QList<QString>> tmpUsersData;
        dbManager.queryTempUserData(tmpUsersData);
        
        for(const QList<QString> &tmpUser : tmpUsersData)
        {
            QString tmpUserUuid = tmpUser[0];
            QString tmpUserImagePath = tmpUser[3];
            
            uint8_t *encryptedTmpUserData = nullptr;
            size_t encryptedTmpUserDataLen = 0;
            
            uint8_t *decryptedTmpUserData = nullptr;
            size_t decryptedTmpUserDataLen = 0;
            
            if(!pSm4->decrypt(tmpUserImagePath, encryptedTmpUserData, encryptedTmpUserDataLen,
                              decryptedTmpUserData, decryptedTmpUserDataLen))
            {
                qDebug()<<"Failed to decrypt temp user data";
                emit sendTmpRegisterResult2(false, "解密临时授权用户人脸数据出错");
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
            
            pCamera->facialComparision(tmpUserImageData, tmpUserImageDataLen, curFaceData, curFaceDataLen, &fSimilarity);
            if(tmpUserImageData != nullptr)
            {
                delete[] tmpUserImageData;
                tmpUserImageData = nullptr;
            }
            
            if(fSimilarity >= similarityThreshold)
            {
                pCamera->freeData(curFaceData);
                curFaceData = nullptr;
                //关闭相机
                pCamera->closeCamera();
                pCamera->uniniCamera();
                emit sendTmpRegisterResult2(false, "该用户已注册临时用户");
                return;
            }
        }
        
        //生成uuid
        QUuid newUuid = QUuid::createUuid();
        uuidString = newUuid.toString(QUuid::WithoutBraces);
        uuidString.remove("-");
        
        tmpUserExist = false;
        
        unsigned char *uuidData = reinterpret_cast<unsigned char*>
                (const_cast<char*>(uuidString.toStdString().c_str()));
    //    unsigned long uuidLen = sizeof(uuidData);
        unsigned long uuidLen = uuidString.size();
        
        unsigned char *result = new unsigned char[uuidLen + curFaceDataLen];
        std::copy(uuidData, uuidData + uuidLen, result);
        std::copy(curFaceData, curFaceData + curFaceDataLen, result + uuidLen);
        
        bool ret = pSm4->encrypt(result, uuidLen + curFaceDataLen, savePath);
        
        if(result != nullptr)
        {
            delete[] result;
            result = nullptr;
        }  
        pCamera->freeData(curFaceData);
        curFaceData = nullptr;   
        
        //关闭相机
        pCamera->closeCamera();
        pCamera->uniniCamera();
        
        QString encryptedPwd;
        QByteArray pwdBytes = userPwd.toUtf8();
        unsigned char *pwdData = reinterpret_cast<unsigned char*>(pwdBytes.data());
        unsigned long pwdLen = pwdBytes.size();  // 不包含结尾的 '\0'
        
        bool ret2 = pSm4->encryptStr(pwdData, pwdLen, encryptedPwd);
        
        if(ret && ret2)
        { 
            bool ret3 = addTmpUserInfo(uuidString, tmpUsername, encryptedPwd, savePath, department); 
            emit sendTmpRegisterResult2(ret3, "");
        }
        else
        {
            emit sendTmpRegisterResult2(false, "");
        }
    }
}

void myViewModel::onChangeTmpUserInfo(QString tmpUserUuid, QString tmpUsername, QString tmpUserPwd, QString department)
{
    QString dirPath = QCoreApplication::applicationDirPath() + "/DetectDir/tempUser";
    QDir().mkpath(dirPath);  // 确保目录存在
    
    if(tmpUserPwd.isEmpty())
    {
        qDebug()<<"tmpUserPwd is empty";
        emit sendTmpRegisterResult2(false, "密码为空");
        return;
    }
    
    QString savePath = dirPath + "/" + tmpUsername + ".enc";
    
    bool tmpUserExist = true;
    
    //先判断该人脸是否已经注册了管理员
    //获取管理员信息
    QString adminUuid;
    QString adminUsername;
    QString adminUserPwd;
    QString adminFaceImagePath;
    QString adminDepartment;
    dbManager.queryAdminData(adminUuid, adminUsername, adminUserPwd, adminFaceImagePath, adminDepartment);
    
    //解密管理员数据
    uint8_t *encryptedAdminData = nullptr;
    size_t encryptedAdminDataLen = 0;
    
    uint8_t *decryptedAdminData = nullptr;
    size_t decrypted_len = 0;
    
    if(!pSm4->decrypt(adminFaceImagePath, encryptedAdminData, encryptedAdminDataLen,
                      decryptedAdminData, decrypted_len))
    {
        qDebug()<<"Failed to decrypt admin data";
        sendTmpRegisterResult2(false, "");
        return;
    }
    
    //剔除uuid
    size_t adminUuidLen = adminUuid.size();
    size_t adminImageDataLen = decrypted_len - adminUuidLen;
    unsigned char *adminImageData = new unsigned char[adminImageDataLen];
    std::copy(decryptedAdminData + adminUuidLen, decryptedAdminData  + adminUuidLen + adminImageDataLen, adminImageData);   
    
    //比对临时用户和管理员人脸相似度
    float fSimilarity;
    pCamera->facialComparision(adminImageData, adminImageDataLen, curFaceData, curFaceDataLen, &fSimilarity);
    if(fSimilarity >= similarityThreshold)
    {
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
        pCamera->freeData(curFaceData);
        curFaceData = nullptr;
        //关闭相机
        pCamera->closeCamera();
        pCamera->uniniCamera();
        emit sendTmpRegisterResult2(false, "该用户人脸已注册管理员权限");
        return;
    }
    
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
    
    //判断当前用户名是否重复，存在则返回
    QString uuidStr;
    dbManager.queryTempUserIdByName(tmpUsername, uuidStr);
    if(tmpUserUuid != uuidStr)
    {
        pCamera->freeData(curFaceData);
        curFaceData = nullptr;
        //关闭相机
        pCamera->closeCamera();
        pCamera->uniniCamera();
        emit sendTmpRegisterResult2(false, "该用户用户名与其他临时用户重复");
        return;
    }
    
    //然后和其他临时用户人脸信息比对
    QList<QList<QString>> tmpUsersData;
    dbManager.queryTempUserData(tmpUsersData);
    
    for(const QList<QString> &tmpUser : tmpUsersData)
    {
        QString tmpUserUuid2 = tmpUser[0];
        if(tmpUserUuid == tmpUserUuid2)
        {
            continue;
        }
        QString tmpUserImagePath = tmpUser[3];
        
        uint8_t *encryptedTmpUserData = nullptr;
        size_t encryptedTmpUserDataLen = 0;
        
        uint8_t *decryptedTmpUserData = nullptr;
        size_t decryptedTmpUserDataLen = 0;
        
        if(!pSm4->decrypt(tmpUserImagePath, encryptedTmpUserData, encryptedTmpUserDataLen,
                          decryptedTmpUserData, decryptedTmpUserDataLen))
        {
            qDebug()<<"Failed to decrypt temp user data";
            emit sendTmpRegisterResult2(false, "解密临时授权用户人脸数据出错");
            return;
        }
        //剔除uuid
        size_t tmpUserUuidLen = tmpUserUuid2.size();
        size_t tmpUserImageDataLen = decryptedTmpUserDataLen - tmpUserUuidLen;
        unsigned char *tmpUserImageData = new unsigned char[tmpUserImageDataLen];
        std::copy(decryptedTmpUserData + tmpUserUuidLen, decryptedTmpUserData  + tmpUserUuidLen + tmpUserImageDataLen, tmpUserImageData);   
        //只保留解密后的图片数据
        if(decryptedTmpUserData != nullptr)
        {
            free(decryptedTmpUserData);
            decryptedTmpUserData = nullptr;
        }
        
        pCamera->facialComparision(tmpUserImageData, tmpUserImageDataLen, curFaceData, curFaceDataLen, &fSimilarity);
        if(tmpUserImageData != nullptr)
        {
            delete[] tmpUserImageData;
            tmpUserImageData = nullptr;
        }
        
        if(fSimilarity >= similarityThreshold)
        {
            pCamera->freeData(curFaceData);
            curFaceData = nullptr;
            //关闭相机
            pCamera->closeCamera();
            pCamera->uniniCamera();
            emit sendTmpRegisterResult2(false, "该用户人脸与其他临时用户重复");
            return;
        }
    }
    
    //更新临时用户信息
    QString uuidString = tmpUserUuid;
    unsigned char *uuidData = reinterpret_cast<unsigned char*>
            (const_cast<char*>(uuidString.toStdString().c_str()));
//    unsigned long uuidLen = sizeof(uuidData);
    unsigned long uuidLen = uuidString.size();
    
    unsigned char *result = new unsigned char[uuidLen + curFaceDataLen];
    std::copy(uuidData, uuidData + uuidLen, result);
    std::copy(curFaceData, curFaceData + curFaceDataLen, result + uuidLen);
    
    bool ret = pSm4->encrypt(result, uuidLen + curFaceDataLen, savePath);
    
    if(result != nullptr)
    {
        delete[] result;
        result = nullptr;
    }  
    pCamera->freeData(curFaceData);
    curFaceData = nullptr;   
    
    //关闭相机
    pCamera->closeCamera();
    pCamera->uniniCamera();
    
    QString encryptedPwd;
    QByteArray pwdBytes = tmpUserPwd.toUtf8();
    unsigned char *pwdData = reinterpret_cast<unsigned char*>(pwdBytes.data());
    unsigned long pwdLen = pwdBytes.size();  // 不包含结尾的 '\0'
    
    bool ret2 = pSm4->encryptStr(pwdData, pwdLen, encryptedPwd);
    
    
    if(ret && ret2)
    { 
        bool ret3 = updateTmpUserInfo(uuidString, tmpUsername, encryptedPwd, savePath, department); 
        qDebug()<<"ret3 "<<ret3;
        emit sendUpdateTmpUsrInfoRes(ret3, tmpUsername);
    }
    else
    {
        emit sendTmpRegisterResult2(false, "");
    }
}

void myViewModel::onGetAllTmpUsrPicInfo()
{
    QList<std::tuple<QString, QString, QPixmap>> decryptedTmpUserList;
    
    QList<QList<QString>> tmpUsersData;
    bool res = dbManager.queryTempUserData(tmpUsersData);
    if(!res)
    {
        qDebug()<<"没有注册临时用户";
        emit sendTmpUsrInfo(decryptedTmpUserList);
        return;
    }
     
    for(const QList<QString> &tmpUser : tmpUsersData)
    {
        QString tmpUserUuid = tmpUser[0];
        QString tmpUserName = tmpUser[1];
        QString tmpUserImagePath = tmpUser[3];
        
        uint8_t *encryptedTmpUserData = nullptr;
        size_t encryptedTmpUserDataLen = 0;
        
        uint8_t *decryptedTmpUserData = nullptr;
        size_t decryptedTmpUserDataLen = 0;
        
        if(!pSm4->decrypt(tmpUserImagePath, encryptedTmpUserData, encryptedTmpUserDataLen,
                          decryptedTmpUserData, decryptedTmpUserDataLen))
        {
            qDebug()<<"Failed to decrypt temp user data";
            return;
        }
        //剔除uuid
        size_t tmpUserUuidLen = tmpUserUuid.size();
        size_t tmpUserImageDataLen = decryptedTmpUserDataLen - tmpUserUuidLen;
        unsigned char *tmpUserImageData = new unsigned char[tmpUserImageDataLen];
        std::copy(decryptedTmpUserData + tmpUserUuidLen, decryptedTmpUserData  + tmpUserUuidLen + tmpUserImageDataLen, tmpUserImageData);   
        //只保留解密后的图片数据
        if(encryptedTmpUserData != nullptr)
        {
            free(encryptedTmpUserData);
            encryptedTmpUserData = nullptr;
        }
        if(decryptedTmpUserData != nullptr)
        {
            free(decryptedTmpUserData);
            decryptedTmpUserData = nullptr;
        }
        
        QPixmap tmpUserPixmap = getPixmap(tmpUserImageData, tmpUserImageDataLen);
        
        //转换为图片后删除原始数据
        if(tmpUserImageData != nullptr)
        {
            free(tmpUserImageData);
            tmpUserImageData = nullptr;
        }
        
        std::tuple<QString, QString, QPixmap> tmpUserTuple(tmpUserUuid, tmpUserName, tmpUserPixmap);
//                QMap<QString, unsigned char *> decryptTmpUerMap;
//                decryptTmpUerMap.insert(tmpUserUuid, tmpUserImageData);
        
        decryptedTmpUserList.append(tmpUserTuple);
    }
    emit sendTmpUsrInfo(decryptedTmpUserList);
}

void myViewModel::onGetTmpUsrPicInfo(QString tmpUsername)
{
    QList<QList<QString>> tmpUserList;
    QList<std::tuple<QString, QString, QPixmap>> decryptedTmpUserList;
    
    bool res = dbManager.queryTempUserDataByName(tmpUsername, tmpUserList);
    if(!res)
    {
        qDebug()<<"没有注册临时用户: " + tmpUsername;
        emit sendTmpUsrInfo(decryptedTmpUserList);
        return;
    }

    for(auto list : tmpUserList)
    {
        QString tmpUserUuid = list[0];
        QString tmpCompleteUsernameComplete = list[1];
        QString tmpUserImagePath = list[2];
        
        uint8_t *encryptedTmpUserData = nullptr;
        size_t encryptedTmpUserDataLen = 0;
        
        uint8_t *decryptedTmpUserData = nullptr;
        size_t decryptedTmpUserDataLen = 0;
        
        if(!pSm4->decrypt(tmpUserImagePath, encryptedTmpUserData, encryptedTmpUserDataLen,
                          decryptedTmpUserData, decryptedTmpUserDataLen))
        {
            qDebug()<<"Failed to decrypt temp user data";
            return;
        }
        //剔除uuid
        size_t tmpUserUuidLen = tmpUserUuid.size();
        size_t tmpUserImageDataLen = decryptedTmpUserDataLen - tmpUserUuidLen;
        unsigned char *tmpUserImageData = new unsigned char[tmpUserImageDataLen];
        std::copy(decryptedTmpUserData + tmpUserUuidLen, decryptedTmpUserData  + tmpUserUuidLen + tmpUserImageDataLen, tmpUserImageData);   
        //只保留解密后的图片数据
        if(encryptedTmpUserData != nullptr)
        {
            free(encryptedTmpUserData);
            encryptedTmpUserData = nullptr;
        }
        if(decryptedTmpUserData != nullptr)
        {
            free(decryptedTmpUserData);
            decryptedTmpUserData = nullptr;
        }
        
        QPixmap tmpUserPixmap = getPixmap(tmpUserImageData, tmpUserImageDataLen);
        
        //转换为图片后删除原始数据
        if(tmpUserImageData != nullptr)
        {
            free(tmpUserImageData);
            tmpUserImageData = nullptr;
        }
        
        std::tuple<QString, QString, QPixmap> tmpUserTuple(tmpUserUuid, 
                                              tmpCompleteUsernameComplete, tmpUserPixmap);
        decryptedTmpUserList.append(tmpUserTuple);
    }
        
    emit sendTmpUsrInfo(decryptedTmpUserList);
}

void myViewModel::onGetTmpUserData(const QString &uuidStr)
{
    QList<QPair<QString, QString> > periodsList;
    bool res = dbManager.queryUsagePeriodsData(uuidStr, periodsList);
    if(res)
    {
        emit sendTmpUserPeriodsData(uuidStr, periodsList);
    }
}

void myViewModel::onDeleteTmpUser(const QString &uuid)
{
    bool res = dbManager.deleteTempUserDataById(uuid);
    emit sendDeleteTmpUserRes(res);
}

void myViewModel::onGetFacePixmap(const QString &tmpUserUuid)
{
    QString tmpUserImagePath;
    bool res = dbManager.queryTempUserImgPathById(tmpUserUuid, tmpUserImagePath);
    if(res && !tmpUserImagePath.isEmpty())
    {
        uint8_t *encryptedTmpUserData = nullptr;
        size_t encryptedTmpUserDataLen = 0;
        
        uint8_t *decryptedTmpUserData = nullptr;
        size_t decryptedTmpUserDataLen = 0;
        
        if(!pSm4->decrypt(tmpUserImagePath, encryptedTmpUserData, encryptedTmpUserDataLen,
                          decryptedTmpUserData, decryptedTmpUserDataLen))
        {
            qDebug()<<"Failed to decrypt temp user data";
            return;
        }
        //剔除uuid
        size_t tmpUserUuidLen = tmpUserUuid.size();
        size_t tmpUserImageDataLen = decryptedTmpUserDataLen - tmpUserUuidLen;
        unsigned char *tmpUserImageData = new unsigned char[tmpUserImageDataLen];
        std::copy(decryptedTmpUserData + tmpUserUuidLen, decryptedTmpUserData  + tmpUserUuidLen + tmpUserImageDataLen, tmpUserImageData);   
        //只保留解密后的图片数据
        if(encryptedTmpUserData != nullptr)
        {
            free(encryptedTmpUserData);
            encryptedTmpUserData = nullptr;
        }
        if(decryptedTmpUserData != nullptr)
        {
            free(decryptedTmpUserData);
            decryptedTmpUserData = nullptr;
        }
        
        QPixmap tmpUserPixmap = getPixmap(tmpUserImageData, tmpUserImageDataLen);
        
        //转换为图片后删除原始数据
        if(tmpUserImageData != nullptr)
        {
            free(tmpUserImageData);
            tmpUserImageData = nullptr;
        }
        
        emit sendFacePixmap(tmpUserPixmap);
    }
    
}

void myViewModel::onDelTmpUserEncFile(QString username)
{
    QString dirPath = QCoreApplication::applicationDirPath() + "/DetectDir/tempUser";
    QDir().mkpath(dirPath);  // 确保目录存在
    
    QString filename = dirPath + "/" + username + ".enc";
    
    QFile file(filename);
    if (!file.remove())
    {
        qDebug() << "删除文件" << filename << "失败:" << file.errorString();
    }
    else
    {
        qDebug() << "删除文件成功";
    }
}

void myViewModel::onAddUserPeriod(const QString &uuid, const QString &startTime, const QString &endTime)
{
    QUuid timmeUuid = QUuid::createUuid();
    QString timeUuidString = timmeUuid.toString(QUuid::WithoutBraces);
    timeUuidString.remove("-");
    
    bool res = dbManager.insertUsagePeriodData(timeUuidString, uuid, startTime, endTime);
    emit sendAddUserPeriodRes(res);
}

void myViewModel::onChangeUserPeriod(const QString &uuid, const QString &oldStartTime, const QString &oldEndTime, const QString &startTime, const QString &endTime)
{
    bool res = dbManager.updateUsagePeriodData(uuid, oldStartTime, oldEndTime, startTime, endTime);
    emit sendChangeUserPeriodRes(res);
}

void myViewModel::onGetAllTmpUsrFaceInfo()
{
    QList<std::tuple<QString, QString, QPixmap>> decryptedTmpUserList;
    
    QList<QList<QString>> tmpUsersData;
    bool res = dbManager.queryTempUserData(tmpUsersData);
    if(!res)
    {
        qDebug()<<"没有注册临时用户";
        emit sendTmpUsrFaceInfo(decryptedTmpUserList);
        return;
    }
     
    for(const QList<QString> &tmpUser : tmpUsersData)
    {
        QString tmpUserUuid = tmpUser[0];
        QString tmpUserName = tmpUser[1];
        QString tmpUserImagePath = tmpUser[3];
        
        uint8_t *encryptedTmpUserData = nullptr;
        size_t encryptedTmpUserDataLen = 0;
        
        uint8_t *decryptedTmpUserData = nullptr;
        size_t decryptedTmpUserDataLen = 0;
        
        if(!pSm4->decrypt(tmpUserImagePath, encryptedTmpUserData, encryptedTmpUserDataLen,
                          decryptedTmpUserData, decryptedTmpUserDataLen))
        {
            qDebug()<<"Failed to decrypt temp user data";
            return;
        }
        //剔除uuid
        size_t tmpUserUuidLen = tmpUserUuid.size();
        size_t tmpUserImageDataLen = decryptedTmpUserDataLen - tmpUserUuidLen;
        unsigned char *tmpUserImageData = new unsigned char[tmpUserImageDataLen];
        std::copy(decryptedTmpUserData + tmpUserUuidLen, decryptedTmpUserData  + tmpUserUuidLen + tmpUserImageDataLen, tmpUserImageData);   
        //只保留解密后的图片数据
        if(encryptedTmpUserData != nullptr)
        {
            free(encryptedTmpUserData);
            encryptedTmpUserData = nullptr;
        }
        if(decryptedTmpUserData != nullptr)
        {
            free(decryptedTmpUserData);
            decryptedTmpUserData = nullptr;
        }
        
        QPixmap tmpUserPixmap = getPixmap(tmpUserImageData, tmpUserImageDataLen);
        
        //转换为图片后删除原始数据
        if(tmpUserImageData != nullptr)
        {
            free(tmpUserImageData);
            tmpUserImageData = nullptr;
        }
        
        std::tuple<QString, QString, QPixmap> tmpUserTuple(tmpUserUuid, tmpUserName, tmpUserPixmap);
//                QMap<QString, unsigned char *> decryptTmpUerMap;
//                decryptTmpUerMap.insert(tmpUserUuid, tmpUserImageData);
        
        decryptedTmpUserList.append(tmpUserTuple);
    }
    emit sendTmpUsrFaceInfo(decryptedTmpUserList);
}

void myViewModel::onGetTmpUsrFaceInfo(QString inputUsername)
{
    QList<QList<QString>> tmpUserList;
    QList<std::tuple<QString, QString, QPixmap>> decryptedTmpUserList;
    
    bool res = dbManager.queryTempUserDataByName(inputUsername, tmpUserList);
    if(!res)
    {
        qDebug()<<"没有注册临时用户: " + inputUsername;
        emit sendTmpUsrFaceInfo(decryptedTmpUserList);
        return;
    }

    for(auto list : tmpUserList)
    {
        QString tmpUserUuid = list[0];
        QString tmpCompleteUsernameComplete = list[1];
        QString tmpUserImagePath = list[2];
        
        uint8_t *encryptedTmpUserData = nullptr;
        size_t encryptedTmpUserDataLen = 0;
        
        uint8_t *decryptedTmpUserData = nullptr;
        size_t decryptedTmpUserDataLen = 0;
        
        if(!pSm4->decrypt(tmpUserImagePath, encryptedTmpUserData, encryptedTmpUserDataLen,
                          decryptedTmpUserData, decryptedTmpUserDataLen))
        {
            qDebug()<<"Failed to decrypt temp user data";
            return;
        }
        //剔除uuid
        size_t tmpUserUuidLen = tmpUserUuid.size();
        size_t tmpUserImageDataLen = decryptedTmpUserDataLen - tmpUserUuidLen;
        unsigned char *tmpUserImageData = new unsigned char[tmpUserImageDataLen];
        std::copy(decryptedTmpUserData + tmpUserUuidLen, decryptedTmpUserData  + tmpUserUuidLen + tmpUserImageDataLen, tmpUserImageData);   
        //只保留解密后的图片数据
        if(encryptedTmpUserData != nullptr)
        {
            free(encryptedTmpUserData);
            encryptedTmpUserData = nullptr;
        }
        if(decryptedTmpUserData != nullptr)
        {
            free(decryptedTmpUserData);
            decryptedTmpUserData = nullptr;
        }
        
        QPixmap tmpUserPixmap = getPixmap(tmpUserImageData, tmpUserImageDataLen);
        
        //转换为图片后删除原始数据
        if(tmpUserImageData != nullptr)
        {
            free(tmpUserImageData);
            tmpUserImageData = nullptr;
        }
        
        std::tuple<QString, QString, QPixmap> tmpUserTuple(tmpUserUuid, 
                                              tmpCompleteUsernameComplete, tmpUserPixmap);
        decryptedTmpUserList.append(tmpUserTuple);
    }
        
    emit sendTmpUsrFaceInfo(decryptedTmpUserList);
}

void myViewModel::onCheckUserFace()
{
    //对比当前人脸信息与管理员人脸信息
    //获取管理员信息
    QString adminUuid;
    QString adminUsername;
    QString adminUserPwd;
    QString adminFaceImagePath;
    QString adminDepartment;
    dbManager.queryAdminData(adminUuid, adminUsername, adminUserPwd, adminFaceImagePath, adminDepartment);
    
    //解密管理员数据
    uint8_t *encryptedAdminData = nullptr;
    size_t encryptedAdminDataLen = 0;
    
    uint8_t *decryptedAdminData = nullptr;
    size_t decrypted_len = 0;
    
    if(!pSm4->decrypt(adminFaceImagePath, encryptedAdminData, encryptedAdminDataLen,
                      decryptedAdminData, decrypted_len))
    {
        qDebug()<<"Failed to decrypt admin data";
        sendTmpRegisterResult2(false, "解密管理员人脸数据出错");
        return;
    }
    
    //剔除uuid
    size_t adminUuidLen = adminUuid.size();
    size_t adminImageDataLen = decrypted_len - adminUuidLen;
    unsigned char *adminImageData = new unsigned char[adminImageDataLen];
    std::copy(decryptedAdminData + adminUuidLen, decryptedAdminData  + adminUuidLen + adminImageDataLen, adminImageData);   
    
    //比对临时用户和管理员人脸相似度
    float fSimilarity;
    pCamera->facialComparision(adminImageData, adminImageDataLen, curFaceData, curFaceDataLen, &fSimilarity);
    
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
    pCamera->freeData(curFaceData);
    curFaceData = nullptr;
    //关闭相机
    pCamera->closeCamera();
    pCamera->uniniCamera();
    
    if(fSimilarity >= similarityThreshold)
    {
        emit sendUserIdentity("admin");
    }
    else
    {
        emit sendUserIdentity("notAdmin");
    }
}

void myViewModel::onCheckUserPwd(QString pwdStr)
{
    QString encryptedPwd;
    bool res1 = dbManager.queryAdminPwd(encryptedPwd);
    if(!res1)
    {
        qWarning()<<"查找管理员密码失败";
        emit sendPwdCheckResult(false);
        return;
    }
    qDebug()<<"encryptedPwd "<<encryptedPwd;

    QByteArray encryptedBytes = QByteArray::fromHex(encryptedPwd.toUtf8());
    unsigned char *encryptedData = reinterpret_cast<unsigned char*>(encryptedBytes.data());
    size_t encryptedLen = encryptedBytes.size();
    
    QString decryptedPwd;
    bool res2 = pSm4->decrptStr(encryptedData, encryptedLen, decryptedPwd);
    qDebug()<<"decryptedPwd "<<decryptedPwd;
    if(!res2)
    {
        qWarning()<<"解密失败";
        emit sendPwdCheckResult(false);
    }
    else
    {
        pwdStr == decryptedPwd ? emit sendPwdCheckResult(true) : emit sendPwdCheckResult(false);
    }
//    QPair<QString, QString> pwdPair = dbManager.queryCurrentPwdConf();
//    if(pwdPair.first == "customPwd")
//    {
//        QString encryptedPwd = pwdPair.second;
//        qDebug()<<"encryptedPwd "<<encryptedPwd;

//        QByteArray encryptedBytes = QByteArray::fromHex(encryptedPwd.toUtf8());
//        unsigned char *encryptedData = reinterpret_cast<unsigned char*>(encryptedBytes.data());
//        size_t encryptedLen = encryptedBytes.size();
        
//        QString decryptedPwd;
//        bool res = pSm4->decrptStr(encryptedData, encryptedLen, decryptedPwd);
//        qDebug()<<"decryptedPwd "<<decryptedPwd;
//        if(!res)
//        {
//            qWarning()<<"解密失败";
//            emit sendPwdCheckResult(false);
//        }
//        else
//        {
//            pwdStr == decryptedPwd ? emit sendPwdCheckResult(true) : emit sendPwdCheckResult(false);
//        }
//    }
//    else if(pwdPair.first == "sysPwd")
//    {
//        QString encryptedPwd;
//        bool res = dbManager.querySysPwd(encryptedPwd);
//        if(!res)
//        {
//            qWarning()<<"获取系统密码失败";
//            emit sendPwdCheckResult(false);
//            return;
//        }
        
//        QByteArray encryptedBytes = QByteArray::fromHex(encryptedPwd.toLatin1());
//        unsigned char *encryptedData = reinterpret_cast<unsigned char*>(encryptedBytes.data());
//        size_t encryptedLen = encryptedBytes.size();
        
//        QString decryptedPwd;
//        bool res2 = pSm4->decrptStr2(encryptedData, encryptedLen, decryptedPwd);
//        qDebug()<<"decryptedPwd: "<<decryptedPwd;
//        if(!res2)
//        {
//            qWarning()<<"解密失败";
//            emit sendPwdCheckResult(false);
//        }
//        else
//        {
//            pwdStr == decryptedPwd ? emit sendPwdCheckResult(true) : emit sendPwdCheckResult(false);            
//        }
//    }
}

void myViewModel::onLogout()
{
//    qDebug()<<"onLogout()";
//    bool res = dbManager.updateLastLogoutTime();
//    if(!res)
//    {
//        qDebug()<<"更新登出时间失败！";
//    }
}
