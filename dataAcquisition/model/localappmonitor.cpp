#include "localappmonitor.h"

LocalAppMonitor::LocalAppMonitor() {
    qDebug() << "LocalAppMonitor initialized";
}

QSet<QString> LocalAppMonitor::getRunningProcesses() {
    QSet<QString> runningProcesses;
    
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        qDebug() << "Failed to create process snapshot";
        return runningProcesses;
    }
    
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    
    if (!Process32First(hProcessSnap, &pe32)) {
        qDebug() << "Failed to get first process";
        CloseHandle(hProcessSnap);
        return runningProcesses;
    }
    
    do {
        QString processName = QString::fromWCharArray(pe32.szExeFile);
        if (isDesktopApp(processName)) {
            runningProcesses.insert(processName);
        }
    } while (Process32Next(hProcessSnap, &pe32));
    
    CloseHandle(hProcessSnap);
    return runningProcesses;
}

bool LocalAppMonitor::isDesktopApp(const QString &appName) {
    // 这里可以根据需要添加更多的桌面应用名称
    QStringList desktopApps = {
        "chrome.exe", "firefox.exe", "msedge.exe", "iexplore.exe",
        "notepad.exe", "word.exe", "excel.exe", "powerpoint.exe",
        "outlook.exe", "teams.exe", "skype.exe", "zoom.exe",
        "photoshop.exe", "paint.exe", "calc.exe", "mspaint.exe",
        "Weixin.exe", "Trae CN.exe", "360net.exe", "NWIM.exe",
        "Xshell.exe", "SuwellReader.exe", "360Safe.exe", "Xftp.exe",
        "Swcure CRT.exe", "Foxmail.exe", "SoftMgr.exe", "et.exe",
        "wpp.exe", "wps.exe", "wpspdf.exe", "智慧办公助手.exe",
        "Night.exe",
    };
    
    return desktopApps.contains(appName, Qt::CaseInsensitive);
}

QList<LocalAppRecord> LocalAppMonitor::getLocalAppInfo() {
    QList<LocalAppRecord> appInfoList;
    QSet<QString> runningProcesses = getRunningProcesses();
    
    for (const QString &appName : runningProcesses) {
        LocalAppRecord info;
        info.localAppName = appName;
        info.status = "active";
        info.startTime = QDateTime::currentDateTime();
        info.endTime = QDateTime();
        info.isActive = true;
        appInfoList.append(info);
    }
    
    return appInfoList;
}

QList<LocalAppRecord> LocalAppMonitor::updateLocalAppRecords() {
    QDateTime currentTime = QDateTime::currentDateTime();
    QSet<QString> currentRunningApps = getRunningProcesses();
    
    qDebug() << "=== Local App Collection Start ===";
    qDebug() << "Current time:" << currentTime.toString("yyyy-MM-dd hh:mm:ss");
    qDebug() << "Current running apps:" << currentRunningApps.size();
    qDebug() << "Running apps:" << currentRunningApps.toList();
    
    // 处理新启动的应用
    for (const QString &appName : currentRunningApps) {
        if (!appRecordMap.contains(appName)) {
            LocalAppRecord record;
            record.localAppName = appName;
            record.startTime = currentTime;
            record.endTime = QDateTime();
            record.status = "active";
            record.isActive = true;
            appRecordMap.insert(appName, record);
            qDebug() << "Added new active app:" << appName << "Start time:" << record.startTime.toString("yyyy-MM-dd hh:mm:ss");
        } else if (!appRecordMap[appName].isActive) {
            // 应用之前结束了，现在重新启动
            LocalAppRecord &record = appRecordMap[appName];
            record.startTime = currentTime;
            record.endTime = QDateTime();
            record.status = "active";
            record.isActive = true;
            qDebug() << "Restarted app:" << appName << "Start time:" << record.startTime.toString("yyyy-MM-dd hh:mm:ss");
        } else {
            qDebug() << "App already active:" << appName;
        }
    }
    
    // 处理已结束的应用
    QList<QString> endedApps;
    for (const QString &appName : appRecordMap.keys()) {
        if (appRecordMap[appName].isActive && !currentRunningApps.contains(appName)) {
            LocalAppRecord &record = appRecordMap[appName];
            record.endTime = currentTime;
            record.status = "inactive";
            record.isActive = false;
            pendingSendRecords.insert(appName, record);
            endedApps.append(appName);
            qDebug() << "App became inactive:" << appName;
            qDebug() << "  Start:" << record.startTime.toString("yyyy-MM-dd hh:mm:ss");
            qDebug() << "  End:" << record.endTime.toString("yyyy-MM-dd hh:mm:ss");
        }
    }
    
    qDebug() << "=== Local App Collection End ===";
    
    lastRunningApps = currentRunningApps;
    return getPendingSendRecords();
}

QList<LocalAppRecord> LocalAppMonitor::getPendingSendRecords() {
    return pendingSendRecords.values();
}

void LocalAppMonitor::clearPendingSendRecords() {
    qDebug() << "Cleared" << pendingSendRecords.size() << "pending send records";
    pendingSendRecords.clear();
}
