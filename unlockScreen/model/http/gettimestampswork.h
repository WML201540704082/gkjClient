#ifndef GETTIMESTAMPSWORK_H
#define GETTIMESTAMPSWORK_H

#include "httpclient.h"

#include <QObject>

class getTimestampsWork : public QObject
{
    Q_OBJECT
public:
    explicit getTimestampsWork(QObject *parent = nullptr);
    
signals:
    void calculateSuccess();
    void workFinished();
    void getTimeStampsFailed();
    
public slots:
    void startSending(QString url);
    
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
private: 
    httpClient* client = nullptr;
    
};

#endif // GETTIMESTAMPSWORK_H
