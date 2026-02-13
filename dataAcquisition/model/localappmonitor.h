#ifndef LOCALAPPMONITOR_H
#define LOCALAPPMONITOR_H

#include <QString>
#include <QDebug>
#include <QDateTime>
#include <QMap>
#include <QSet>
#include <windows.h>
#include <tlhelp32.h>



struct LocalAppRecord {
    QString localAppName;
    QDateTime startTime;
    QDateTime endTime;
    QString status;
    bool isActive;
};

class LocalAppMonitor {
public:
    LocalAppMonitor();
    
    QList<LocalAppRecord> getLocalAppInfo();
    
    QList<LocalAppRecord> updateLocalAppRecords();
    
    QList<LocalAppRecord> getPendingSendRecords();
    
    void clearPendingSendRecords();

private:
    bool isDesktopApp(const QString &appName);
    
    QSet<QString> getRunningProcesses();
    
    QSet<QString> lastRunningApps;
    
    QMap<QString, LocalAppRecord> appRecordMap;
    
    QMap<QString, LocalAppRecord> pendingSendRecords;
};

#endif // LOCALAPPMONITOR_H