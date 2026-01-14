#ifndef SENDPOWERONOFFWORK_H
#define SENDPOWERONOFFWORK_H

#include "model/myStruct.h"
#include "model/http/httpclient.h"

#include <QObject>

class sendPowerOnOffWork : public QObject
{
    Q_OBJECT
public:
    explicit sendPowerOnOffWork(QObject *parent = nullptr);
    ~sendPowerOnOffWork();
    
    
    void recheckTimestamps();
    
public slots:
    ///
    /// \brief startSending 开始发送登录日志信息
    /// \param logList
    ///
    void startSending(const QList<powerOnOffTimeInfo> &timeList);
    
    ///
    /// \brief handleSuccess 发送成功收到回复后处理响应码
    /// \param response 回复内容
    ///
    void handleSuccess(const QByteArray &response);
    
    ///
    /// \brief handleError 发送失败记录错误
    /// \param errorString
    ///
    void handleError(const QString &errorString);
    
    ///
    /// \brief handleFail 重传达到最大次数
    ///
    void handleFail();
    
    ///
    /// \brief onCalculateSuccess 对时成功槽函数
    ///
    void onCalculateSuccess();
    
signals:
    void startGetTimestamps(QString url);
    void sendTimeInfoSuccessfully(QList<powerOnOffTimeInfo> timeList);
    
private:
    httpClient* client = nullptr;
    
    QList<powerOnOffTimeInfo> curTimeList;
};

#endif // SENDPOWERONOFFWORK_H
