#ifndef SENDLOCALAPPINFOWORK_H
#define SENDLOCALAPPINFOWORK_H

#include "model/http/httpclient.h"

#include <QObject>

class LocalAppMonitor;

class sendLocalAppInfoWork : public QObject {
    Q_OBJECT
public:
    explicit sendLocalAppInfoWork(QObject *parent = nullptr);
    ~sendLocalAppInfoWork();
    
    void recheckTimestamps();
    
public slots:
    void startSending();
    void handleSuccess(const QByteArray &response);
    void handleError(const QString &errorString);
    void handleFail();
    void onCalculateSuccess();
    
signals:
    void startGetTimestamps(QString url);
    
private:
    httpClient* client = nullptr;
    LocalAppMonitor* monitor = nullptr;
};

#endif // SENDLOCALAPPINFOWORK_H