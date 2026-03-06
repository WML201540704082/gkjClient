#ifndef SENDUDISKINFOWORK_H
#define SENDUDISKINFOWORK_H

#include <QObject>

class sendUDiskInfoWork : public QObject {
    Q_OBJECT
public:
    explicit sendUDiskInfoWork(QObject *parent = nullptr);
    ~sendUDiskInfoWork();

public slots:
    void startSending();
    void onCalculateSuccess();

signals:
    void startGetTimestamps(QString url);

private slots:
    void recheckTimestamps();
    void handleSuccess(const QByteArray &response);
    void handleError(const QString &errorString);
    void handleFail();

private:
    class httpClient *client = nullptr;
    class UDiskMonitor *monitor = nullptr;
};

#endif // SENDUDISKINFOWORK_H
