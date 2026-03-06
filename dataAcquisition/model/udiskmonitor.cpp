#include "udiskmonitor.h"
#include <QDir>

UDiskMonitor::UDiskMonitor() {
    qDebug() << "UDiskMonitor initialized";
}

QSet<QString> UDiskMonitor::getConnectedUDisks() {
    QSet<QString> connectedUDisks;
    
    // 遍历所有驱动器字母
    for (char drive = 'A'; drive <= 'Z'; drive++) {
        QString driveLetter = QString("%1:/").arg(drive);
        
        // 检查是否是可移动磁盘
        UINT driveType = GetDriveTypeA(driveLetter.toUtf8().constData());
        if (driveType == DRIVE_REMOVABLE) {
            // 获取U盘卷标
            char volumeName[MAX_PATH + 1];
            if (GetVolumeInformationA(driveLetter.toUtf8().constData(), volumeName, MAX_PATH + 1, NULL, NULL, NULL, NULL, 0)) {
                QString uDiskName = QString("%1 (%2:)").arg(volumeName).arg(drive);
                connectedUDisks.insert(uDiskName);
            } else {
                QString uDiskName = QString("(%1:)").arg(drive);
                connectedUDisks.insert(uDiskName);
            }
        }
    }
    
    return connectedUDisks;
}

bool UDiskMonitor::isSafeUDisk(const QString &driveLetter) {
    // 从U盘名称中提取驱动器字母，例如 "USB (E:)" -> "E:"
    QString drive = driveLetter;
    int driveIndex = drive.indexOf('(');
    if (driveIndex != -1) {
        int driveEndIndex = drive.indexOf(')', driveIndex);
        if (driveEndIndex != -1) {
            drive = drive.mid(driveIndex + 1, driveEndIndex - driveIndex - 1);
        }
    }
    
    // 检查U盘中是否有EdpEDisk.exe
    QString edpExePath = drive + "/EdpEDisk.exe";
    QFileInfo edpExeInfo(edpExePath);
    bool hasEdpExe = edpExeInfo.exists() && edpExeInfo.isFile();
    
    qDebug() << "Checking UDisk:" << driveLetter;
    qDebug() << "  Checking path:" << edpExePath;
    qDebug() << "  Has EdpEDisk.exe:" << hasEdpExe;
    
    return hasEdpExe;
}

QList<UDiskInfo> UDiskMonitor::getUDiskInfo() {
    QList<UDiskInfo> uDiskInfoList;
    QSet<QString> connectedUDisks = getConnectedUDisks();
    
    for (const QString &uDiskName : connectedUDisks) {
        UDiskInfo info;
        info.UDiskName = uDiskName;
        info.isEncryptedUDisk = isSafeUDisk(uDiskName) ? "true" : "false";
        info.startTime = QDateTime::currentDateTime();
        info.endTime = QDateTime();
        uDiskInfoList.append(info);
    }
    
    return uDiskInfoList;
}

QList<UDiskRecord> UDiskMonitor::updateUDiskRecords() {
    QDateTime currentTime = QDateTime::currentDateTime();
    QSet<QString> currentConnectedUDisks = getConnectedUDisks();
    
    qDebug() << "=== UDisk Collection Start ===";
    qDebug() << "Current time:" << currentTime.toString("yyyy-MM-dd hh:mm:ss");
    qDebug() << "Current connected UDisks:" << currentConnectedUDisks.size();
    qDebug() << "Connected UDisks:" << currentConnectedUDisks.toList();
    
    // 处理新连接的U盘
    for (const QString &uDiskName : currentConnectedUDisks) {
        if (!uDiskRecordMap.contains(uDiskName)) {
            UDiskRecord record;
            record.UDiskName = uDiskName;
            record.isEncryptedUDisk = isSafeUDisk(uDiskName) ? "true" : "false";
            record.startTime = currentTime;
            record.endTime = QDateTime();
            record.isActive = true;
            uDiskRecordMap.insert(uDiskName, record);
            qDebug() << "Added new active UDisk:" << uDiskName;
            qDebug() << "  Is safe UDisk:" << record.isEncryptedUDisk;
            qDebug() << "  Start time:" << record.startTime.toString("yyyy-MM-dd hh:mm:ss");
        } else if (!uDiskRecordMap[uDiskName].isActive) {
            // U盘之前拔出了，现在重新插入
            UDiskRecord &record = uDiskRecordMap[uDiskName];
            record.isEncryptedUDisk = isSafeUDisk(uDiskName) ? "true" : "false";
            record.startTime = currentTime;
            record.endTime = QDateTime();
            record.isActive = true;
            qDebug() << "Reconnected UDisk:" << uDiskName;
            qDebug() << "  Is safe UDisk:" << record.isEncryptedUDisk;
            qDebug() << "  Start time:" << record.startTime.toString("yyyy-MM-dd hh:mm:ss");
        } else {
            qDebug() << "UDisk already active:" << uDiskName;
        }
    }
    
    // 处理已拔出的U盘
    QList<QString> disconnectedUDisks;
    for (const QString &uDiskName : uDiskRecordMap.keys()) {
        if (uDiskRecordMap[uDiskName].isActive && !currentConnectedUDisks.contains(uDiskName)) {
            UDiskRecord &record = uDiskRecordMap[uDiskName];
            record.endTime = currentTime;
            record.isActive = false;
            pendingSendRecords.insert(uDiskName, record);
            disconnectedUDisks.append(uDiskName);
            qDebug() << "UDisk disconnected:" << uDiskName;
            qDebug() << "  Is safe UDisk:" << record.isEncryptedUDisk;
            qDebug() << "  Start:" << record.startTime.toString("yyyy-MM-dd hh:mm:ss");
            qDebug() << "  End:" << record.endTime.toString("yyyy-MM-dd hh:mm:ss");
        }
    }
    
    qDebug() << "=== UDisk Collection End ===";
    
    lastConnectedUDisks = currentConnectedUDisks;
    return getPendingSendRecords();
}

QList<UDiskRecord> UDiskMonitor::getPendingSendRecords() {
    return pendingSendRecords.values();
}

void UDiskMonitor::clearPendingSendRecords() {
    qDebug() << "Cleared" << pendingSendRecords.size() << "pending UDisk send records";
    pendingSendRecords.clear();
}
