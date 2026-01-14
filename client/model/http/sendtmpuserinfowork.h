#ifndef SENDTMPUSERINFOWORK_H
#define SENDTMPUSERINFOWORK_H

#include "model/http/httpclient.h"
#include "model/myStruct.h"

#include <QObject>

class sendTmpUserInfoWork : public QObject
{
    Q_OBJECT
public:
    explicit sendTmpUserInfoWork(QObject *parent = nullptr);
    ~sendTmpUserInfoWork();
    
    void recheckTimestamps();
    
    void processData(QJsonValue value);
    
public slots:
    ///
    /// \brief startSending 开始发送临时用户信息
    /// \param adminList
    ///
    void startSending(const QList<tmpUserInfo> &tmpUserList);
    
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
    void sendTmpUserInfoSuccessfully(const QList<tmpUserInfo> &serverTmpUserList, const QList<tmpUserInfo> &tmpUserList);
    void startGetTimestamps(QString url);
    
private:
    QList<tmpUserInfo> curTmpUserList;
    
    httpClient* client = nullptr;  
};

#endif // SENDTMPUSERINFOWORK_H
