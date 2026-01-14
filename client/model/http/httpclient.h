#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QUrl>

class httpClient : public QObject
{
    Q_OBJECT
public:
    explicit httpClient(QObject *parent = nullptr);
    
    ///
    /// \brief get 设置url并调用makeGetRequest
    /// \param url url地址
    ///
    void get(const QUrl &url);
    
    ///
    /// \brief post 设置url、数据并调用makePostRequest
    /// \param url url地址
    /// \param data 数据
    ///
    void post(const QUrl &url, const QByteArray &data);
    
    
signals:
    void requestSuccess(const QByteArray &response);
    void requestError(const QString &errorString);
    void sendFail();
    
private slots:
    void onFinished(QNetworkReply *reply);
    
    ///
    /// \brief onTimeout 超时槽函数，重传上一个请求
    ///
    void onTimeout();
    
private:
    ///
    /// \brief makeGetRequest 调用get请求
    ///
    void makeGetRequest();
    
    ///
    /// \brief makePostRequest 调用post请求
    ///
    void makePostRequest();
    
    ///
    /// \brief setupTimeout 初始化定时器，设置超时时间
    ///
    void setupTimeout();
    
    QNetworkAccessManager *manager;
    QNetworkReply *reply;
    QTimer *timeoutTimer;
    QUrl url;
    QByteArray data;
    int retryCount;
    const int maxRetries;
};


#endif // HTTPCLIENT_H
