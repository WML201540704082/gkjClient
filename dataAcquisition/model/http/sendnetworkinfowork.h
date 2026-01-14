#ifndef SENDNETWORKINFOWORK_H
#define SENDNETWORKINFOWORK_H

#include "model/http/httpclient.h"

#include <QObject>

class sendNetworkInfoWork : public QObject
{
    Q_OBJECT
public:
    explicit sendNetworkInfoWork(QObject *parent = nullptr);
    ~ sendNetworkInfoWork();
    
    void recheckTimestamps();
    
public slots:
    ///
    /// \brief startSending 发送网络信息槽函数
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
    void startGetTimestamps(QString url);
    
private:
    httpClient* client = nullptr;
};

#endif // SENDNETWORKINFOWORK_H
