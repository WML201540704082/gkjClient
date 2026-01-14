#ifndef MYVIEWMODEL_H
#define MYVIEWMODEL_H

#include "./model/databasemanager.h"
#include "dvtcamera.h"
#include "./model/mysm4.h"
#include "model/caminiwork.h"
#include "checkanddecryptwork.h"
#include "utf8.h"
#include "model/syncwork.h"

#include <QObject>
#include <QPixmap>
#include <QThread>

class myViewModel : public QObject
{
    Q_OBJECT
public:
    explicit myViewModel(QObject *parent = nullptr);
    ~myViewModel();
    
    ///
    /// \brief iniThread 初始化线程
    ///
    void iniThread();
    
    ///
    /// \brief delThread 删除线程
    ///
    void delThread();
    
    ///
    /// \brief compareFaces 对比人脸
    ///
    void compareFaces();
    
    ///
    /// \brief dataToPixmap 将相机传来的数据转换为图片
    ///
    void  dataToPixmap(unsigned char* faceData, unsigned long faceDataLen);
    
    ///
    /// \brief updateIniFile
    /// \param str
    ///
    void updateIniFile(QString str);
    
    ///
    /// \brief delTmpUserList 清楚临时用户列表
    ///
    void delTmpUserList();
    
    ///
    /// \brief recordLogin
    /// \param username
    ///
    void recordLogin(const QString &username, const QString &department);
    
    ///
    /// \brief pwdLoginPassed 密码验证通过
    ///
    void pwdLoginPassed();
    
    ///
    /// \brief iniClientConfig 初始化客户端配置表数据
    ///
    void iniClientConfig();
    
    ///
    /// \brief checkLoginLog 检查登录日志记录，清除一年前记录
    ///
    void checkLoginLog();
    
    ///
    /// \brief isFirstLogin 判断是否是首次登录
    /// \return
    ///
    bool isFirstLogin();
    
    ///
    /// \brief isPwdChecked 判断是否验证过密码
    /// \return 
    ///
    bool isPwdChecked();
    
public slots:    
    ///
    /// \brief iniDatabase 初始化数据库
    ///
    void onIniDatabase();
    
    ///
    /// \brief onThreadFinished
    ///
    void onThreadErrorFinished(QString errStr);
    
    ///
    /// \brief onCheckAdminData 检查是否注册了管理员
    ///
    void onCheckAdminData();
    
    ///
    /// \brief onSendAdminData
    ///
    void onSendAdminData(unsigned char *data, size_t dataLen, QString username, QString pwd, QString department);
    
    ///
    /// \brief onSendTmpUserData
    /// \param adminData
    /// \param decryptedTmpUserList
    ///
    void onSendTmpUserData(unsigned char *adminData, size_t adminDataLen,
                           QString username, QString pwd, QString department,
                           QList<std::tuple<QString, unsigned char *, size_t, QString, QString, QString>> decryptedTmpUserList);

    ///
    /// \brief onIniCam 初始化相机槽函数
    ///
    void onIniCam();

    ///
    /// \brief onOpenCam 打开相机槽函数
    ///
    void onOpenCam(HWND m_hWnd, int nVideoWindowWidth, int nVideoWindowHeight);
    
    ///
    /// \brief onStartDetection 开始人脸检测槽函数
    ///
    void onStartDetection();

    ///
    /// \brief onSendFaceData
    /// \param faceData
    /// \param faceDataLen
    ///
    void onSendFaceData(unsigned char* faceData, unsigned long faceDataLen);
    
    ///
    /// \brief onSendCamMessage
    /// \param message
    ///
    void onSendCamMessage(QString message);
    
    ///
    /// \brief onGetAllTmpUserData 获取所有临时用户的数据
    ///
    void onGetAllTmpUserData();
    
    ///
    /// \brief onFacialAuthenticationPassed
    ///
    void onFacialAuthenticationPassed();
    
    ///
    /// \brief onKeyboardLogin 使用开机密码登录
    ///
    void onKeyboardLogin();
    
    ///
    /// \brief onPwdLogin 使用用户名密码登录
    /// \param identity
    ///
    void onPwdLogin(QString identity, QString username, QString department);
    
    ///
    /// \brief onIniSyncThread 初始化同步线程
    ///
    void onIniSyncThread();

signals:
    void startCheckAndDecrypt();
    void noRegisteredAdmin();
    void checkThreadErrorFinished(QString errStr);
    
    void repostCamMessage(QString message);
    void repostCamMessageFloating(QString message);
    void sendPixmap(QPixmap pixmap);
    void sendVmMessage(QString message);
    void sendAllTmpUserData(QList<QPair<QString, QPair<QString, QString> > > dataList);
    void sendAvailableUserInfo(QString adminUsername, QString adminPwd, QPixmap adminPixmap, QString adminDepartment);
    void sendAvailableUserInfo2(QString adminUsername, QString adminPwd, QPixmap adminPixmap, QString adminDepartment,
                               QList<std::tuple<QString, QString, QPixmap, QString>> tmpUserInfo);
    void startIniCam(DvtCamera *pCamera);
    
    void iniSyncWork();
    void startSyncWork();
    
private:
    databaseManager dbManager;
    
    QString adminUuid;
    QString adminImagePath;

    DvtCamera *pCamera = nullptr;
    unsigned char* curFaceData;
    unsigned long curFaceDataLen;  
    
    checkAndDecryptWork *checkDecryptWork = nullptr;
    QThread *myThread = nullptr;
    
    camIniWork *pCamIniWork = nullptr;
    QThread *camIniThread = nullptr;
    
    bool compareAdminDataOnly = false;
    unsigned char *adminImageData = nullptr;
    size_t adminImageDataLen = 0;
    QString adminUsername;
    QString adminPwd;
    QString adminDepartment;
    QList<std::tuple<QString, unsigned char *, size_t, QString, QString, QString>> tmpUserList;

    syncWork *pSyncWork = nullptr;
    QThread *syncThread = nullptr;
};

#endif // MYVIEWMODEL_H
