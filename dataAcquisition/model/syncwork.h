#ifndef SYNCWORK_H
#define SYNCWORK_H

#include "model/http/gettimestampswork.h"
#include "model/http/sendpoweronoffwork.h"
#include "model/http/sendnetworkinfowork.h"

#include <QObject>

class syncWork : public QObject
{
    Q_OBJECT
public:
    explicit syncWork(QObject *parent = nullptr);
    ~syncWork();
    
    void sendSyncData();
    
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
    /// \brief onSendTimeInfoSuccessfully
    /// \param timeList
    ///
    void onSendTimeInfoSuccessfully(QList<powerOnOffTimeInfo> timeList);
    
signals:
    void startGetTimestamps(QString url);
    void startSendTimeInfo(const QList<powerOnOffTimeInfo> & timeList);
    void startSendNetworkInfo();
    
private:
    getTimestampsWork* timestampsWork = nullptr;
    QThread* getTimestampsThread = nullptr;
    
    sendPowerOnOffWork* powerOnOffWork = nullptr;
    QThread* sendPowerOnOffThread = nullptr;
    
    sendNetworkInfoWork* networkInfoWork = nullptr;
    QThread* sendNetworkInfoThread = nullptr;
    
    QTimer *timer = nullptr; 
    QTimer *timer2 = nullptr; 
};

#endif // SYNCWORK_H
