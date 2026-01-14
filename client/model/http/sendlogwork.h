#ifndef SENDLOGWORK_H
#define SENDLOGWORK_H

#include "model/http/httpclient.h"
#include "model/myStruct.h"

#include <QObject>

class sendLogWork : public QObject
{
    Q_OBJECT
public:
    explicit sendLogWork(QObject *parent = nullptr);
    ~sendLogWork();
    
    void recheckTimestamps();
    
public slots:
    ///
    /// \brief startSending 开始发送登录日志信息
    /// \param logList
    ///
    void startSending(const QList<loginInfo> &logList);
    
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
    void sendLogInfoSuccessfully(const QList<loginInfo> &adminList);
    void startGetTimestamps(QString url);
    
private:
    QList<loginInfo> curLogList;
    
    httpClient* client = nullptr;  
};

#endif // SENDLOGWORK_H
