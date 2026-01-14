#include "myviewmodel.h"
#include "utf8.h"

#include <QImage>
#include <QDebug>
#include <QDir>
#include <QUuid>
#include <QCoreApplication>
#include <QSettings>
#include <QTimer>
#include <QMessageBox>
#include <QApplication>

//人脸相似度阈值
float similarityThreshold = 85.0;

// 共享变量（使用 std::atomic 以确保线程安全）
std::atomic<bool> decryptDone(false);

myViewModel::myViewModel(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<QList<std::tuple<QString, unsigned char*, size_t, QString, QString, QString>>>
            ("QList<std::tuple<QString,unsigned char*,size_t,QString,QString,QString>>");
    
    
    pCamera = new DvtCamera();
    connect(pCamera, &DvtCamera::sendFaceData, this, &myViewModel::onSendFaceData);
    connect(pCamera, &DvtCamera::sendCamMessage, this, &myViewModel::onSendCamMessage);     
    
    iniThread();
}

myViewModel::~myViewModel()
{
    // 如果线程仍在运行，清理它
    if(myThread && myThread->isRunning())
    {
        myThread->quit();
        myThread->wait();
    }
    delete myThread;
    delete checkDecryptWork;
}

void myViewModel::iniThread()
{
    checkDecryptWork = new checkAndDecryptWork();
    myThread = new QThread();
    
    checkDecryptWork->moveToThread(myThread);
    
    connect(myThread, &QThread::finished, checkDecryptWork, &checkAndDecryptWork::deleteLater);
    connect(this, &myViewModel::startCheckAndDecrypt, checkDecryptWork, &checkAndDecryptWork::onStartCheckAndDecrypt);
    connect(checkDecryptWork, &checkAndDecryptWork::threadErrorFinished, this, &myViewModel::onThreadErrorFinished);
    connect(checkDecryptWork, &checkAndDecryptWork::sendAdminData, this, &myViewModel::onSendAdminData);
    connect(checkDecryptWork, &checkAndDecryptWork::sendTmpUserData, this, &myViewModel::onSendTmpUserData);
    myThread->start();
    
    pCamIniWork = new camIniWork();
    camIniThread = new QThread();
    
    pCamIniWork->moveToThread(camIniThread);
    
    connect(camIniThread, &QThread::finished, pCamIniWork, &camIniWork::deleteLater);
    qRegisterMetaType<DvtCamera*>("DvtCamera*");
    connect(this, &myViewModel::startIniCam, pCamIniWork, &camIniWork::onStartIniCam);
}

void myViewModel::delThread()
{
    qDebug()<<"delThread()";
    if(myThread->isRunning())
    {
        myThread->quit();  // 停止线程
        myThread->wait();  // 等待线程结束
    }
    delete myThread;
    myThread = nullptr;  // 避免悬空指针
}

void myViewModel::compareFaces()
{
    qDebug()<<"compareFaces()";
    // 在主线程启动定时器
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this, timer]() {
        if (decryptDone)
        {
            qDebug()<<"compareAdminDataOnly："<<compareAdminDataOnly;
            if(compareAdminDataOnly)
            {
                //比对当前人脸和管理员用户人脸
                float fSimilarity;
                pCamera->facialComparision(adminImageData, adminImageDataLen, curFaceData, curFaceDataLen, &fSimilarity);
                if(fSimilarity >= similarityThreshold)
                {
                    if(adminImageData != nullptr)
                    {
                        delete adminImageData;
                        adminImageData = nullptr;
                    }
                    updateIniFile("admin");
                    recordLogin(adminUsername, adminDepartment);
                    timer->stop();
                    dataToPixmap(curFaceData, curFaceDataLen);           
                }
                else
                {
                    timer->stop();
                    onStartDetection();
                }
            }
            else
            {
                //先比对当前人脸和管理员用户人脸
                float fSimilarity;
                pCamera->facialComparision(adminImageData, adminImageDataLen, curFaceData, curFaceDataLen, &fSimilarity);
                if(fSimilarity >= similarityThreshold)
                {
                    if(adminImageData != nullptr)
                    {
                        delete adminImageData;
                        adminImageData = nullptr;
                    }
                    updateIniFile("admin");
                    recordLogin(adminUsername, adminDepartment);
                    timer->stop();
                    dataToPixmap(curFaceData, curFaceDataLen);
                }
                //不匹配则依次比对临时用户人脸
                else
                {
                    qDebug()<<"tmpUserList size: "<<tmpUserList.size();
                    for(std::tuple<QString, unsigned char *, size_t, QString, QString, QString> &tmpuser : tmpUserList)
                    {
                        QString tmpUserUuid = std::get<0>(tmpuser);
                        unsigned char * tmpUserImageData = std::get<1>(tmpuser);
                        size_t tmpUserImageDataLen = std::get<2>(tmpuser);
                        QString tmpUserDepartment = std::get<5>(tmpuser);
                        pCamera->facialComparision(tmpUserImageData, tmpUserImageDataLen, curFaceData, curFaceDataLen, &fSimilarity);
                        if(fSimilarity >= similarityThreshold)
                        {
                            delTmpUserList();
                            
                            QString tmpUsername;
                            bool res = dbManager.queryTempUserNameById(tmpUserUuid, tmpUsername);
                            if(res)
                            {
                                updateIniFile(tmpUsername);
                                recordLogin(tmpUsername, tmpUserDepartment);
                            }
                            else
                            {
                                qDebug()<<"查找临时用户名失败!";
                            }
                            
                            timer->stop();
                            dataToPixmap(curFaceData, curFaceDataLen);
                            return;
                        }
                    }
                    timer->stop();
                    onStartDetection();
                }
            }     
        }
    });
    timer->start(100); // 每 100ms 检查一次    
}

void myViewModel::onCheckAdminData()
{
    emit startCheckAndDecrypt();
}

void myViewModel::onSendAdminData(unsigned char *data, size_t dataLen, QString username, QString pwd, QString department)
{
    decryptDone = true;
    compareAdminDataOnly = true;
    
    adminImageData = data;
    adminImageDataLen = dataLen;
    adminUsername = username;
    adminPwd = pwd;
    adminDepartment = department;
    
    //把管理员人脸数据转为图片
    QByteArray byteArray(reinterpret_cast<char*>(adminImageData), adminImageDataLen);  
    QImage image = QImage::fromData(byteArray);
    if (image.isNull()) 
    {
        // 处理图像加载失败的情况
        qWarning() << "Failed to load image from data!";
        emit sendVmMessage("处理管理员人脸图片失败");
        return;
    }
    QPixmap pixmap = QPixmap::fromImage(image);
    
    emit sendAvailableUserInfo(adminUsername, adminPwd, pixmap, adminDepartment);
    
    delThread();
    
//    QString tmpFileName ="D:/Qt5.12.9Project/build-faceRecognition-Desktop_Qt_5_12_9_MSVC2017_64bit-Release/lnsoft-otfr/DetectDir/image.jpg";
//    QFile file(tmpFileName);
//    if (!file.open(QIODevice::WriteOnly))
//    {
//        qDebug() << "Cannot open file for writing!";
//        return ;
//    }
    
//    file.write(reinterpret_cast<const char*>(adminImageData), adminImageDataLen);
    //    file.close();
}

void myViewModel::onSendTmpUserData(unsigned char *adminData, size_t adminDataLen, QString username, QString pwd, QString department, QList<std::tuple<QString, unsigned char *, size_t, QString, QString, QString> > decryptedTmpUserList)
{
    qDebug()<<"onSendTmpUserData";
    decryptDone = true;
    
    adminImageData = adminData;
    adminImageDataLen = adminDataLen;
    adminUsername = username;
    adminPwd = pwd;
    adminDepartment = department;
    tmpUserList = decryptedTmpUserList;
    
    //把管理员人脸数据转为图片
    QByteArray adminByteArray(reinterpret_cast<char*>(adminImageData), adminImageDataLen);  
    QImage adminImage = QImage::fromData(adminByteArray);
    if (adminImage.isNull()) 
    {
        // 处理图像加载失败的情况
        qWarning() << "Failed to load image from data!";
        emit sendVmMessage("处理管理员人脸图片失败");
        return;
    }
    QPixmap adminPixmap = QPixmap::fromImage(adminImage);
    
    //把临时用户人脸数据转为图片
    QList<std::tuple<QString, QString, QPixmap, QString>> tmpUserInfoList;
    for(std::tuple<QString, unsigned char *, size_t, QString, QString, QString> &tmpuser : tmpUserList)
    {
        unsigned char * tmpUserImageData = std::get<1>(tmpuser);
        size_t tmpUserImageDataLen = std::get<2>(tmpuser);
        QString tmpUsername = std::get<3>(tmpuser);
        QString tmpUserPwd = std::get<4>(tmpuser);
        QString tmpUserDepartment = std::get<5>(tmpuser);
        QByteArray tmpUserByteArray(reinterpret_cast<char*>(tmpUserImageData), tmpUserImageDataLen);
        QImage tmpUserImage = QImage::fromData(tmpUserByteArray);
        if (tmpUserImage.isNull()) 
        {
            // 处理图像加载失败的情况
            qWarning() << "Failed to load image from temp user data!";
            emit sendVmMessage("处理临时用户人脸图片失败");
            continue;
        }
        QPixmap tmpUserPixmap = QPixmap::fromImage(tmpUserImage);
        
        std::tuple<QString, QString, QPixmap, QString> tmpUserTuple(tmpUsername, tmpUserPwd, tmpUserPixmap, tmpUserDepartment);
        tmpUserInfoList.append(tmpUserTuple);
    }
    
    emit sendAvailableUserInfo2(adminUsername, adminPwd, adminPixmap, adminDepartment, tmpUserInfoList);
    
    delThread();
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

void myViewModel::updateIniFile(QString str)
{
//    QFile file("./loginConfig.ini");
//    if (!file.exists())
//    {
//        qDebug()<<"loginConfig.ini not exist";
//        return;
//    }
//    else
//    {
//        QSettings *pSettings = new QSettings("./loginConfig.ini", QSettings::IniFormat);
//        pSettings->setValue("ExeConfig/LoginIdentity", str);   
//    }
    
    databaseManager dbManager;
    dbManager.connectToDatabase();
    bool res = dbManager.updateLoginIdentity(str);
    if(!res)
    {
        qDebug()<<"updateLoginIdentity failed";
    }
}

void myViewModel::delTmpUserList()
{
    for(std::tuple<QString, unsigned char *, size_t, QString, QString, QString> &tmpuser : tmpUserList)
    {
        unsigned char * tmpUserImagedata = std::get<1>(tmpuser);
        delete tmpUserImagedata;
        tmpUserImagedata = nullptr;
    }
    tmpUserList.clear();
}

void myViewModel::recordLogin(const QString &username, const QString &department)
{
    databaseManager dbManager;
    dbManager.connectToDatabase();
    dbManager.insertLoginLogData(username, department);
}

void myViewModel::pwdLoginPassed()
{
    qDebug()<<"pwdLoginPassed()";
    //获取系统密码
    databaseManager dbManager;
    QString username;
    QString pwd;
    dbManager.connectToDatabase();
    dbManager.querySysData(username, pwd);
    
    qDebug()<<username;
    qDebug()<<pwd;
    
    //管道通信发送加密后的密码
    HANDLE hPipe = CreateFileW(
        L"\\\\.\\pipe\\MyCredentialPipe",
        GENERIC_WRITE,
        0,
        nullptr,
        OPEN_EXISTING,
        0,
        nullptr
    );
    if (hPipe != INVALID_HANDLE_VALUE)
    {
        DWORD bytesWritten;
        
        //将密码从QSring转为uint8_t *
        QByteArray encryptedPassword = QByteArray::fromHex(pwd.toUtf8());
        uint8_t *encryptedPwdData = reinterpret_cast<uint8_t*>(const_cast<char*>(encryptedPassword.constData()));
        size_t encrypted_len = encryptedPassword.size();
        
        WriteFile(
            hPipe,
            encryptedPwdData,  // 获取二进制数据指针
            encrypted_len,       // 获取数据长度
            &bytesWritten,
            nullptr
        );
        
        CloseHandle(hPipe);
    } 
    else
    {
        DWORD dwError = GetLastError();
        wchar_t errorMsg[256];
        swprintf_s(errorMsg, L"CreateFileW failed with error 0x%08X", dwError);
        QMessageBox::critical(nullptr, "Error", QString::fromWCharArray(errorMsg));
    }

    // 退出程序
    QApplication::quit();
}

void myViewModel::iniClientConfig()
{
    bool res = dbManager.insertConfigData(false, "");
    if(!res)
    {
        qDebug()<<"初始化客户端配置表数据失败";
    }
}

void myViewModel::checkLoginLog()
{
    dbManager.cleanOldLoginLogs(365);
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

void myViewModel::onIniDatabase()
{
    dbManager.connectToDatabase();
    dbManager.createTables();
    
    iniClientConfig();
    checkLoginLog();
}

void myViewModel::onThreadErrorFinished(QString errStr)
{
    delThread();
    emit checkThreadErrorFinished(errStr);
}


void myViewModel::onIniCam()
{
//    pCamera = new DvtCamera();

//    connect(pCamera, &DvtCamera::sendFaceData, this, &myViewModel::onSendFaceData);
//    connect(pCamera, &DvtCamera::sendCamMessage, this, &myViewModel::onSendCamMessage);
    
    if(!camIniThread->isRunning())
    {
        camIniThread->start();
    }
    emit startIniCam(pCamera);
    
//    pCamera->iniCamera();
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
    
    compareFaces();
}

void myViewModel::onSendCamMessage(QString message)
{
    emit repostCamMessage(message);
}

void myViewModel::onGetAllTmpUserData()
{
    QList<QPair<QString, QPair<QString, QString> > > dataList = 
                                            dbManager.getAllTemporaryUsersWithUsagePeriods();
    
    emit sendAllTmpUserData(dataList);
}

void myViewModel::onFacialAuthenticationPassed()
{
    //释放数据    
    pCamera->freeData(curFaceData);
    curFaceData = nullptr;
    
    //关闭相机
    pCamera->closeCamera();
    pCamera->uniniCamera();
    
    
    //获取系统密码
    databaseManager dbManager;
    QString username;
    QString pwd;
    dbManager.connectToDatabase();
    dbManager.querySysData(username, pwd);
    
    qDebug()<<username;
    qDebug()<<pwd;
    
    //管道通信发送加密后的密码
    HANDLE hPipe = CreateFileW(
        L"\\\\.\\pipe\\MyCredentialPipe",
        GENERIC_WRITE,
        0,
        nullptr,
        OPEN_EXISTING,
        0,
        nullptr
    );
    if (hPipe != INVALID_HANDLE_VALUE)
    {
        DWORD bytesWritten;
        
        //将密码从QSring转为uint8_t *
        QByteArray encryptedPassword = QByteArray::fromHex(pwd.toUtf8());
        uint8_t *encryptedPwdData = reinterpret_cast<uint8_t*>(const_cast<char*>(encryptedPassword.constData()));
        size_t encrypted_len = encryptedPassword.size();
        
        WriteFile(
            hPipe,
            encryptedPwdData,  // 获取二进制数据指针
            encrypted_len,       // 获取数据长度
            &bytesWritten,
            nullptr
        );
        
        CloseHandle(hPipe);
    } 
    else
    {
        DWORD dwError = GetLastError();
        wchar_t errorMsg[256];
        swprintf_s(errorMsg, L"CreateFileW failed with error 0x%08X", dwError);
        QMessageBox::critical(nullptr, "Error", QString::fromWCharArray(errorMsg));
    }

    // 退出程序
    QApplication::quit();
}

void myViewModel::onKeyboardLogin()
{
    updateIniFile("keyboard");
}

void myViewModel::onPwdLogin(QString identity, QString username, QString department)
{
    updateIniFile(identity);
    recordLogin(username, department);
    pwdLoginPassed();
}

void myViewModel::onIniSyncThread()
{
    pSyncWork = new syncWork();
    syncThread = new QThread();
    
    pSyncWork->moveToThread(syncThread);
    
    connect(syncThread, &QThread::finished, pSyncWork, &syncWork::deleteLater);
    connect(this, &myViewModel::iniSyncWork, pSyncWork, &syncWork::onIniSyncWork);
    connect(this, &myViewModel::startSyncWork, pSyncWork, &syncWork::onStartSyncWork);
    connect(pSyncWork, &syncWork::syncFinished, this, &myViewModel::onCheckAdminData);
    
    qDebug() << "main thread id: " << QThread::currentThreadId();
    
    syncThread->start();
    
    emit iniSyncWork();
    emit startSyncWork();
}
