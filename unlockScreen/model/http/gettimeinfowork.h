#ifndef GETTIMEINFOWORK_H
#define GETTIMEINFOWORK_H

#include "model/http/httpclient.h"
#include "model/myStruct.h"

#include <QObject>

class getTimeInfoWork : public QObject
{
    Q_OBJECT
public:
    explicit getTimeInfoWork(QObject *parent = nullptr);
    ~getTimeInfoWork();
    
    void recheckTimestamps();
    
    void processData(QJsonValue value);
    
public slots:
    ///
    /// \brief startSending 开始发送用户授权时间信息
    ///
    void startSending();
    
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
    void getUsageTimeInfoSuccessfully(const QList<usageTime> &serverTimeList);
    void startGetTimestamps(QString url);
    
private:  
    httpClient* client = nullptr;  
};

#endif // GETTIMEINFOWORK_H
