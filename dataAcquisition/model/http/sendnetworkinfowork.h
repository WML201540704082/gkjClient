#ifndef SENDNETWORKINFOWORK_H
#define SENDNETWORKINFOWORK_H

#include "model/http/httpclient.h"

#include <QObject>

class networkMonitor;

class sendNetworkInfoWork : public QObject {
    Q_OBJECT
public:
    explicit sendNetworkInfoWork(QObject *parent = nullptr);
    ~sendNetworkInfoWork();
    
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
    networkMonitor* monitor = nullptr;
};

#endif // SENDNETWORKINFOWORK_H