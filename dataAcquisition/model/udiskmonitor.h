#ifndef UDISKMONITOR_H
#define UDISKMONITOR_H

#include <QString>
#include <QDebug>
#include <QDateTime>
#include <QMap>
#include <QSet>
#include <windows.h>

struct UDiskInfo {
    QString UDiskName; // U盘名称
    QString isEncryptedUDisk; // 是否是安全U盘，true or false
    QDateTime startTime; // 插入时间
    QDateTime endTime; // 拔出时间
};

struct UDiskRecord {
    QString UDiskName;
    QString isEncryptedUDisk;
    QDateTime startTime;
    QDateTime endTime;
    bool isActive;
};

class UDiskMonitor {
public:
    UDiskMonitor();
    
    QList<UDiskInfo> getUDiskInfo();
    
    QList<UDiskRecord> updateUDiskRecords();
    
    QList<UDiskRecord> getPendingSendRecords();
    
    void clearPendingSendRecords();

private:
    bool isSafeUDisk(const QString &driveLetter);
    
    QSet<QString> getConnectedUDisks();
    
    QSet<QString> lastConnectedUDisks;
    
    QMap<QString, UDiskRecord> uDiskRecordMap;
    
    QMap<QString, UDiskRecord> pendingSendRecords;
};

#endif // UDISKMONITOR_H
