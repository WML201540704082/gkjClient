#ifndef SENDTIMEINFOWORK_H
#define SENDTIMEINFOWORK_H

#include "model/http/httpclient.h"
#include "model/myStruct.h"

#include <QObject>

class sendTimeInfoWork : public QObject
{
    Q_OBJECT
public:
    explicit sendTimeInfoWork(QObject *parent = nullptr);
    ~sendTimeInfoWork();
    
    void recheckTimestamps();
    
    void processData(QJsonValue value);
    
public slots:
    ///
    /// \brief startSending 开始发送用户授权时间信息
    /// \param usageTimeList
    ///
    void startSending(const QList<usageTime> &usageTimeList);
    
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
    void sendUsageTimeInfoSuccessfully(const QList<usageTime> &serverTimeList, const QList<usageTime> &usageTimeList);
    void startGetTimestamps(QString url);
    
private:
    QList<usageTime> curUsageTimeList;
    
    httpClient* client = nullptr;  
};

#endif // SENDTIMEINFOWORK_H
