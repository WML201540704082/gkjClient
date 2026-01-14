#ifndef SYNCWORK_H
#define SYNCWORK_H

#include "model/http/gettimestampswork.h"
#include "model/http/sendadmininfowork.h"
#include "model/http/sendtmpuserinfowork.h"
#include "model/http/sendtimeinfowork.h"
#include "model/http/sendlogwork.h"
#include "model/myStruct.h"

#include <QObject>
#include <QThread>

class syncWork : public QObject
{
    Q_OBJECT
public:
    explicit syncWork(QObject *parent = nullptr);
    
    void sendSyncData();
    
    bool deleteUserFile(QString username);
    
public slots:
    ///
    /// \brief onIniSyncWork 初始化同步线程
    ///
    void onIniSyncWork();
    
    ///
    /// \brief onStartSyncWork 开始同步操作
    ///
    void onStartSyncWork();
    
    ///
    /// \brief receiveCalculateSuccess 收到对时成功槽函数
    ///
    void receiveCalculateSuccess();
    
    ///
    /// \brief onSendAdminInfoSuccessfully 发送管理员信息成功槽函数
    ///
    void onSendAdminInfoSuccessfully(QList<adminInfo> serverAdminList, const QList<adminInfo> &adminList);
    
    ///
    /// \brief onSendTmpUserInfoSuccessfully 发送临时用户信息成功槽函数
    /// \param serverTmpUserList
    /// \param tmpUserList
    ///
    void onSendTmpUserInfoSuccessfully(const QList<tmpUserInfo> &serverTmpUserList, const QList<tmpUserInfo> &tmpUserList);
    
    ///
    /// \brief onSendUsageTimeInfoSuccessfully 发送用户授权时间信息成功槽函数
    /// \param usageTimeList
    ///
    void onSendUsageTimeInfoSuccessfully(const QList<usageTime> &serverTimeList, const QList<usageTime> &usageTimeList);
    
    ///
    /// \brief onSendLogInfoSuccessfully 发送用户登录日志信息成功槽函数
    /// \param logList
    ///
    void onSendLogInfoSuccessfully(const QList<loginInfo> &logList);
    
signals:
    void startGetTimestamps(QString url);
    void startSendAdminInfo(const QList<adminInfo> &adminList);
    void startSendTmpUserInfo(const QList<tmpUserInfo> &tmpUserList);
    void startSendUsageTimeInfo(const QList<usageTime> &usageTimeList);
    void startSendLogInfo(const QList<loginInfo> &logList);
    
private:
    bool firstSend = true;
    
    getTimestampsWork* timestampsWork = nullptr;
    QThread* getTimestampsThread = nullptr;
    
    sendAdminInfoWork* adminInfoWork = nullptr;
    QThread* sendAdminInfoWorkThread = nullptr;
    
    sendTmpUserInfoWork* tmpUserInfoWork = nullptr;
    QThread* sendTmpUserInfoWorkThread = nullptr;
    
    sendTimeInfoWork* usageTimeInfoWork = nullptr;
    QThread* sendUsageTimeInfoWorkThread = nullptr;
    
    sendLogWork* logInfoWork = nullptr;
    QThread* sendLogInfoWorkThread = nullptr;
};

#endif // SYNCWORK_H
