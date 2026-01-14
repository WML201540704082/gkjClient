#ifndef MYVIEWMODEL_H
#define MYVIEWMODEL_H

#include "./model/databasemanager.h"
#include "dvtcamera.h"
#include "./model/mysm4.h"
#include "./model/pwdcheck.h"
#include "./model/screenstatuscheck.h"
#include "model/syncwork.h"
#include "model/caminiwork.h"

#include <QObject>
#include <QPixmap>

class myViewModel : public QObject
{
    Q_OBJECT
public:
    explicit myViewModel(QObject *parent = nullptr);
    ~myViewModel();

    ///
    /// \brief iniClientConfig 初始化客户端配置表数据
    ///
    void iniClientConfig();
    
    ///
    /// \brief isFirstLogin 判断是否是首次登录
    /// \return
    ///
    bool isFirstLogin();
    
    ///
    /// \brief checkLoginLog 检查登录日志记录，清除一年前记录
    ///
    void checkLoginLog();
    
    ///
    /// \brief isPwdChecked 判断是否验证过密码
    /// \return 
    ///
    bool isPwdChecked();
    
    ///
    /// \brief dataToPixmap 将相机传来的数据转换为图片
    /// \param faceData
    /// \param faceDataLen
    ///
    void dataToPixmap(unsigned char* faceData, unsigned long faceDataLen);
    
    ///
    /// \brief getPixmap 将图像数据转换成pixmap
    /// \param faceData
    /// \param faceDataLen
    /// \return 
    ///
    QPixmap getPixmap(unsigned char* faceData, unsigned long faceDataLen);
    
    ///
    /// \brief writeEncryptedDataToFile 加密数据写入文件
    /// \param data
    /// \param len
    /// \param filename
    /// \return 
    ///
    bool writeEncryptedDataToFile(const uint8_t *data, size_t len, const QString &filename);

    ///
    /// \brief addTmpUserInfo 添加临时用户信息
    /// \param tmpUserExist
    /// \param uuid
    /// \param username
    /// \param userPwd
    /// \param faceImagePath
    /// \param startTime
    /// \param endTime
    /// \param department
    /// \return 
    ///
    bool addTmpUserInfo(const bool &tmpUserExist, const QString &uuid, const QString &username, 
                        const QString &userPwd, const QString &faceImagePath, const QString &startTime,
                        const QString &endTime, const QString &department);
    
    ///
    /// \brief addTmpUserInfo 添加临时用户信息
    /// \param uuid
    /// \param username
    /// \param userPwd
    /// \param faceImagePath
    /// \param department
    /// \return 
    ///
    bool addTmpUserInfo(const QString &uuid, const QString &username,
                        const QString &userPwd, const QString &faceImagePath, const QString &department);
    
    ///
    /// \brief checkSysUserAndPwd 检查系统用户名和密码
    /// \param username
    /// \param pwd
    /// \return 
    ///
    bool checkSysUserAndPwd(QString username, QString pwd);
    
    ///
    /// \brief saveSysUserAndPwd 存储系统用户名和密码
    /// \param username
    /// \param pwd
    /// \return 
    ///
    bool saveSysUserAndPwd(QString username, QString pwd);
    
    ///
    /// \brief updateTmpUserInfo 更新临时用户信息
    /// \param uuid
    /// \param username
    /// \param userPwd
    /// \param faceImagePath
    /// \param department
    /// \return 
    ///
    bool updateTmpUserInfo(const QString &uuid, const QString &username, const QString &userPwd, const QString &faceImagePath, const QString &department);
    
    ///
    /// \brief getLastLoginIdentity 获取上次登陆用户身份
    /// \return 
    ///
    QString getLastLoginIdentity();
    
public slots:
    ///
    /// \brief onIniCheckThread 初始化锁屏检查槽函数
    ///
    void onIniCheckThread();
    
    ///
    /// \brief onIniSyncThread 初始化同步线程槽函数
    ///
    void onIniSyncThread();
    
    ///
    /// \brief iniDatabase 初始化数据库
    ///
    void onIniDatabase();

    ///
    /// \brief onIniCam 初始化相机槽函数
    ///
    void onIniCam();
    
    ///
    /// \brief onUniniCam 释放
    ///
    void onUniniCam();

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
    /// \brief onUserRegistration 用户注册槽函数
    /// \param user
    /// \param pwd
    /// \param department
    ///
    void onUserRegistration(QString user, QString pwd, QString department);
    
    ///
    /// \brief onUserReregistration 用户重新注册槽函数
    /// \param user
    /// \param pwd
    /// \param department
    ///
    void onUserReregistration(QString user, QString pwd, QString department);
    
    ///
    /// \brief onCheckUserAndPwd 检查系统用户名密码并保存槽函数
    /// \param username
    /// \param pwd
    ///
    void onCheckUserAndPwd(QString username, QString pwd);
    
    ///
    /// \brief onCheckUserAndPwdOnly 检查系统用户名密码槽函数
    /// \param username
    /// \param pwd
    ///
    void onCheckUserAndPwdOnly(QString username, QString pwd);
    
    ///
    /// \brief onSaveUserAndPwd 存储系统用户名密码槽函数
    /// \param username
    /// \param pwd
    ///
    void onSaveUserAndPwd(QString username, QString pwd);
    
    ///
    /// \brief onCheckUsername 检查管理员表用户名
    ///
    void onCheckUsername();
    
    ///
    /// \brief onGetAllTmpUserData 获取所有临时用户的数据
    ///
    void onGetAllTmpUserData();
    
    ///
    /// \brief onGetTempUserLogin 获取是否运行临时用户登录
    ///
    void onGetTempUserLogin();
    
    ///
    /// \brief onSetTempUserLogin 设置是否允许临时用户登录
    /// \param tmpUserLogin
    ///
    void onSetTempUserLogin(bool tmpUserLogin);
    
    ///
    /// \brief onDeleteTmpUserData 删除临时用户数据
    /// \param tmpUserName
    /// \param startTime
    /// \param endTime
    /// \param index
    ///
    void onDeleteTmpUserData(int index, QString tmpUserName, QString startTime, QString endTime);
        
    ///
    /// \brief onChangeTmpUserPeriod 更新临时用户时间段数据
    /// \param tmpUserName
    /// \param startTime
    /// \param endTime
    ///
    void onChangeTmpUserPeriod(const QString &tmpUserName,const QString &startTime, const QString &endTime);
    
    ///
    /// \brief onDeleteTmpUserData2
    /// \param index
    /// \param uuidStr
    /// \param startTime
    /// \param endTime
    ///
    void onDeleteTmpUserData2(int index, const QString& uuidStr, const QString& startTime, const QString& endTime);    
    
    ///
    /// \brief onTmpUserRegistration 临时用户注册槽函数
    /// \param username
    /// \param userPwd
    /// \param startTime
    /// \param endTime
    /// \param department
    ///
    void onTmpUserRegistration(QString username, QString userPwd, QString startTime, QString endTime, QString department);
    
    ///
    /// \brief onTmpUserRegistration2 临时用户注册槽函数
    /// \param tmpUsername
    /// \param userPwd
    /// \param department
    ///
    void onTmpUserRegistration2(QString tmpUsername, QString userPwd, QString department);
    
    ///
    /// \brief onChangeTmpUserInfo
    /// \param tmpUserUuid
    /// \param tmpUsername
    /// \param tmpUserPwd
    /// \param department
    ///
    void onChangeTmpUserInfo(QString tmpUserUuid, QString tmpUsername, QString tmpUserPwd, QString department);
    
    ///
    /// \brief onGetAllTmpUsrPicInfo
    ///
    void onGetAllTmpUsrPicInfo();
    
    ///
    /// \brief onGetTmpUsrPicInfo
    /// \param tmpUsername
    ///
    void onGetTmpUsrPicInfo(QString tmpUsername);
    
    ///
    /// \brief onGetTmpUserData
    /// \param uuidStr
    ///
    void onGetTmpUserData(const QString& uuidStr);
    
    ///
    /// \brief onDeleteTmpUser
    /// \param uuid
    ///
    void onDeleteTmpUser(const QString& uuid);
    
    ///
    /// \brief onGetFacePixmap
    /// \param tmpUserUuid
    ///
    void onGetFacePixmap(const QString& tmpUserUuid);
    
    ///
    /// \brief onDelTmpUserEncFile
    /// \param username
    ///
    void onDelTmpUserEncFile(QString username);

    ///
    /// \brief onAddUserPeriod
    /// \param uuid
    /// \param startTime
    /// \param endTime
    ///
    void onAddUserPeriod(const QString &uuid, const QString &startTime, const QString &endTime);
      
    ///
    /// \brief onChangeUserPeriod
    /// \param uuid
    /// \param oldStartTime
    /// \param oldEndTime
    /// \param startTime
    /// \param endTime
    ///
    void onChangeUserPeriod(const QString &uuid, const QString &oldStartTime, const QString &oldEndTime, const QString &startTime, const QString &endTime);
       
    ///
    /// \brief onGetAllTmpUsrFaceInfo
    ///
    void onGetAllTmpUsrFaceInfo();
    
    ///
    /// \brief onGetTmpUsrFaceInfo
    /// \param inputUsername
    ///
    void onGetTmpUsrFaceInfo(QString inputUsername);
    
    ///
    /// \brief onCheckUserFace
    ///
    void onCheckUserFace();
    
    ///
    /// \brief onCheckUserPwd
    /// \param pwdStr
    ///
    void onCheckUserPwd(QString pwdStr);
    
    ///
    /// \brief onLogout
    ///
    void onLogout();
    
signals:
    void repostCamMessage(QString message);
    void repostCamMessageFloating(QString message);
    void sendPixmap(QPixmap pixmap);
    void sendVmMessage(QString message);
    void sendRegisterResult(bool result);
    void sendCheckResult(bool result);
    void sendCheckOnlyResult(bool result);
    void refreshUsername(QString username);
    void sendAllTmpUserData(QList<QPair<QString, QPair<QString, QString> > > dataList);
    void sendTmpRegisterResult(bool result, QString str);
    void sendTmpUserLogin(bool tmpUserLogin);
    void sendDelTmpUserDataResult(bool result, int index);
    void sendTmpUsrInfo(QList<std::tuple<QString, QString, QPixmap>> list);
    void sendTmpRegisterResult2(bool result, QString str);
    void sendTmpUserPeriodsData(const QString& uuidStr, QList<QPair<QString, QString> > dataList);
    void sendDeleteTmpUserRes(bool res);
    void sendFacePixmap(QPixmap pixmap);
    void sendUpdateTmpUsrInfoRes(bool result, QString newUsername);
    void sendAddUserPeriodRes(bool result);
    void sendDelTmpUserDataResult2(bool result, int index);
    void sendChangeUserPeriodRes(bool result);
    void changeTmpUserPeriod2(const QString& uuidStr, const QString& tmpUsername, QPixmap tmpPixmap,
                              const QString& startTime, const QString& endTime);
    
    void sendTmpUsrFaceInfo(QList<std::tuple<QString, QString, QPixmap>> list);
    void sendSetConfigPwdRes(bool result);
    void sendCurConf(QPair<QString, QString> confPair);
    void sendUserIdentity(QString identityStr);
    void sendPwdCheckResult(bool result);
    void checkStart();
    void iniSyncWork();
    void startSyncWork();
    void startIniCam(DvtCamera *pCamera);
    
private:
    databaseManager dbManager;

    DvtCamera *pCamera = nullptr;
    unsigned char* curFaceData;
    unsigned long curFaceDataLen;  
    
    screenStatusCheck *screenStatusCheckWork = nullptr;
    QThread *checkThread = nullptr;
    
    syncWork *pSyncWork = nullptr;
    QThread *syncThread = nullptr;
    
    camIniWork *pCamIniWork = nullptr;
    QThread *camIniThread = nullptr;
    
    mySm4 *pSm4 = nullptr;

};

#endif // MYVIEWMODEL_H
