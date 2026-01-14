#ifndef BOOTTIME_H
#define BOOTTIME_H

#include <QObject>

class bootTime : public QObject
{
    Q_OBJECT
public:
    explicit bootTime(QObject *parent = nullptr);
    
    QDateTime parseWMIDateTime(const QString &wmiTime);
    QDateTime getBootTimeViaWMI();
    
signals:
    
};

#endif // BOOTTIME_H
