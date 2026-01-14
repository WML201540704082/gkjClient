#ifndef GETADMININFOWORK_H
#define GETADMININFOWORK_H

#include "model/http/httpclient.h"
#include "model/myStruct.h"

#include <QObject>

class getAdminInfoWork : public QObject
{
    Q_OBJECT
public:
    explicit getAdminInfoWork(QObject *parent = nullptr);
    ~getAdminInfoWork();
    
    void recheckTimestamps();
    
    void processData(QJsonValue value);
    
public slots:
    ///
    /// \brief startSending 开始发送管理员信息
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
    void getAdminInfoSuccessfully(QList<adminInfo> serverAdminList);
    void startGetTimestamps(QString url);
    
private:    
    httpClient* client = nullptr; 
    
};

#endif // GETADMININFOWORK_H
