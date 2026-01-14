#ifndef SYNCWORK_H
#define SYNCWORK_H

#include "model/http/gettimestampswork.h"
#include "model/http/getadmininfowork.h"
#include "model/http/gettmpuserinfowork.h"
#include "model/http/gettimeinfowork.h"

#include <QObject>

class syncWork : public QObject
{
    Q_OBJECT
public:
    explicit syncWork(QObject *parent = nullptr);
    
    void getSyncData();
    
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
    /// \brief onGetAdminInfoSuccessfully 获取管理员信息成功槽函数
    /// \param serverAdminList
    ///
    void onGetAdminInfoSuccessfully(QList<adminInfo> serverAdminList);
    
    ///
    /// \brief onGetTmpUserInfoSuccessfully
    /// \param serverTmpUserList
    ///
    void onGetTmpUserInfoSuccessfully(const QList<tmpUserInfo> &serverTmpUserList);
    
    ///
    /// \brief onGetUsageTimeInfoSuccessfully
    /// \param serverTimeList
    ///
    void onGetUsageTimeInfoSuccessfully(const QList<usageTime> &serverTimeList);
    
    ///
    /// \brief onGetTimeStampsFailed
    ///
    void onGetTimeStampsFailed();
    
signals:
    void startGetTimestamps(QString url);
    void startGetAdminInfo();
    void startGetTmpUserInfo();
    void startGetUsageTimeInfo();
    void syncFinished();
    
private:
    bool firstSend = true;
    
    getTimestampsWork* timestampsWork = nullptr;
    QThread* getTimestampsThread = nullptr;
    
    getAdminInfoWork* adminInfoWork = nullptr;
    QThread* getAdminInfoWorkThread = nullptr;
    
    getTmpUserInfoWork* tmpUserInfoWork = nullptr;
    QThread* getTmpUserInfoWorkThread = nullptr;
    
    getTimeInfoWork* usageTimeInfoWork = nullptr;
    QThread* getUsageTimeInfoWorkThread = nullptr;
    
};

#endif // SYNCWORK_H
