#include "networkmonitor.h"

#include <QProcess>

networkMonitor::networkMonitor()
{
}

QList<ConnectionInfo> networkMonitor::getTcpConnections()
{
    QList<ConnectionInfo> connections;
    
    DWORD size = 0;
    GetExtendedTcpTable(NULL, &size, FALSE, AF_INET, 
                        TCP_TABLE_OWNER_PID_ALL, 0);
    
    PMIB_TCPTABLE_OWNER_PID tcpTable = 
            (PMIB_TCPTABLE_OWNER_PID)malloc(size);
    
    if (GetExtendedTcpTable(tcpTable, &size, FALSE, AF_INET,
                            TCP_TABLE_OWNER_PID_ALL, 0) == NO_ERROR)
    {
        for (DWORD i = 0; i < tcpTable->dwNumEntries; i++)
        {
            ConnectionInfo info;
            info.protocol = "TCP";
            
            struct in_addr localAddr;
            localAddr.S_un.S_addr = tcpTable->table[i].dwLocalAddr;
            info.localAddress = QString(inet_ntoa(localAddr));
            info.localPort = ntohs((u_short)tcpTable->table[i].dwLocalPort);
            
            struct in_addr remoteAddr;
            remoteAddr.S_un.S_addr = tcpTable->table[i].dwRemoteAddr;
            info.remoteAddress = QString(inet_ntoa(remoteAddr));
            info.remotePort = ntohs((u_short)tcpTable->table[i].dwRemotePort);
            
            info.state = getTcpState(tcpTable->table[i].dwState);
            
            info.processId = tcpTable->table[i].dwOwningPid;
                        
            connections.append(info);
        }
    }
    
    free(tcpTable);
    return connections;
}

QList<ConnectionInfo> networkMonitor::getUdpConnections()
{
    QList<ConnectionInfo> connections;
    
    DWORD size = 0;
    GetExtendedUdpTable(NULL, &size, FALSE, AF_INET,
                        UDP_TABLE_OWNER_PID, 0);
    
    PMIB_UDPTABLE_OWNER_PID udpTable = 
            (PMIB_UDPTABLE_OWNER_PID)malloc(size);
    
    if (GetExtendedUdpTable(udpTable, &size, FALSE, AF_INET,
                            UDP_TABLE_OWNER_PID, 0) == NO_ERROR)
    {
        for (DWORD i = 0; i < udpTable->dwNumEntries; i++)
        {
            ConnectionInfo info;
            info.protocol = "UDP";
            
            struct in_addr localAddr;
            localAddr.S_un.S_addr = udpTable->table[i].dwLocalAddr;
            info.localAddress = QString(inet_ntoa(localAddr));
            info.localPort = ntohs((u_short)udpTable->table[i].dwLocalPort);
            
            info.remoteAddress = "*";
            info.remotePort = 0;
            info.state = "N/A";
            info.processId = udpTable->table[i].dwOwningPid;
            
            connections.append(info);
        }
    }
    
    free(udpTable);
    return connections;
}

QList<ConnectionInfo> networkMonitor::getAllConnections()
{
    QList<ConnectionInfo> allConnections;
    
    QList<ConnectionInfo> tcpConnections = getTcpConnections();
    allConnections.append(tcpConnections);
    
    QList<ConnectionInfo> udpConnections = getUdpConnections();
    for (int i = 0; i < udpConnections.size(); i++) {
        const ConnectionInfo conn = udpConnections[i];
        if (conn.remoteAddress != "*") {
            allConnections.append(conn);
        }
    }
    
    return allConnections;
}

bool networkMonitor::isInternalIP(const QString &ip)
{
    if (ip == "127.0.0.1" || ip == "localhost") {
        return true;
    }
    
    QStringList parts = ip.split('.');
    if (parts.size() != 4) {
        return false;
    }
    
    bool ok;
    int first = parts[0].toInt(&ok);
    if (!ok) return false;
    
    int second = parts[1].toInt(&ok);
    if (!ok) return false;
    
    if (first == 10) {
        return true;
    } else if (first == 172 && (second >= 16 && second <= 31)) {
        return true;
    } else if (first == 192 && second == 168) {
        return true;
    }
    
    return false;
}

QString networkMonitor::generateTupleKey(const ConnectionInfo &conn)
{
    return QString("%1_%2:%3_%4:%5")
        .arg(conn.protocol)
        .arg(conn.localAddress)
        .arg(conn.localPort)
        .arg(conn.remoteAddress)
        .arg(conn.remotePort);
}

QList<IPAccessRecord> networkMonitor::updateIPAccessRecords()
{
    QDateTime currentTime = QDateTime::currentDateTime();
    QSet<QString> currentConnectionTuples;
    QSet<QString> externalIPs;
    QMap<QString, QString> connectionStates;
    
    QList<ConnectionInfo> allConnections = getAllConnections();
    
//    qDebug() << "=== Network Collection Start ===";
//    qDebug() << "Total connections collected:" << allConnections.size();
    
    for(int i = 0; i < allConnections.size(); i++) {
        const ConnectionInfo conn = allConnections[i];
        if (conn.remoteAddress == "*" || conn.remoteAddress.isEmpty()) {
            continue;
        }
        
        if (!isInternalIP(conn.remoteAddress)) {
            QString tupleKey = generateTupleKey(conn);
            currentConnectionTuples.insert(tupleKey);
            externalIPs.insert(conn.remoteAddress);
            
            connectionStates.insert(tupleKey, conn.state);
            
//            qDebug() << "Collected external connection:";
//            qDebug() << "  Protocol:" << conn.protocol;
//            qDebug() << "  Local:" << conn.localAddress << ":" << conn.localPort;
//            qDebug() << "  Remote:" << conn.remoteAddress << ":" << conn.remotePort;
//            qDebug() << "  State:" << conn.state;
//            qDebug() << "  Tuple key:" << tupleKey;
        }
    }
    
//    qDebug() << "External IPs found:" << externalIPs.size();
//    qDebug() << "External connection tuples:" << currentConnectionTuples.size();
    
    QList<QString> keys = connectionStates.keys();
    for (int i = 0; i < keys.size(); i++) {
        QString key = keys[i];
        QString state = connectionStates[key];
        
        QStringList parts = key.split('_');
        if (parts.size() == 3) {
            QString ipPart = parts[2];
            int colonIndex = ipPart.indexOf(':');
            if (colonIndex != -1) {
                QString ip = ipPart.left(colonIndex);
                
                bool isActiveState = true;
                if (state == "CLOSED" || state == "TIME_WAIT" || state == "FIN_WAIT2" || state == "CLOSE_WAIT" || state == "LAST_ACK") {
                    isActiveState = false;
                }
                
                if (isActiveState) {
                    if (!ipAccessMap.contains(ip)) {
                        IPAccessRecord record;
                        record.ip = ip;
                        record.startTime = currentTime;
                        record.isActive = true;
                        ipAccessMap.insert(ip, record);
//                        qDebug() << "Added new active IP:" << ip << "Start time:" << record.startTime.toString("yyyy-MM-dd hh:mm:ss");
                    } else {
//                        qDebug() << "IP already active:" << ip;
                    }
                } else {
//                    qDebug() << "Skipping inactive connection for IP:" << ip << "State:" << state;
                }
            }
        }
    }
    
//    qDebug() << "=== Connection State Analysis ===";
//    qDebug() << "Last connection tuples size:" << lastConnectionTuples.size();
    
    if (!lastConnectionTuples.isEmpty()) {
        QSet<QString> commonTuples = lastConnectionTuples.intersect(currentConnectionTuples);
        QSet<QString> closedTuples = lastConnectionTuples - commonTuples;
        QSet<QString> newTuples = currentConnectionTuples - commonTuples;
        
//        qDebug() << "Common tuples:" << commonTuples.size();
//        qDebug() << "Closed tuples:" << closedTuples.size();
//        qDebug() << "New tuples:" << newTuples.size();
        
        QList<QString> closedTupleList = closedTuples.toList();
        for (int i = 0; i < closedTupleList.size(); i++) {
            QString closedTuple = closedTupleList[i];
            QStringList parts = closedTuple.split('_');
            if (parts.size() == 3) {
                QString ipPart = parts[2];
                int colonIndex = ipPart.indexOf(':');
                if (colonIndex != -1) {
                    QString ip = ipPart.left(colonIndex);
                    
//                    qDebug() << "Processing closed connection:";
//                    qDebug() << "  Tuple:" << closedTuple;
//                    qDebug() << "  IP:" << ip;
                    
                    bool stillActive = false;
                    QList<QString> currentTupleList = currentConnectionTuples.toList();
                    for (int j = 0; j < currentTupleList.size(); j++) {
                        QString currentTuple = currentTupleList[j];
                        QStringList currentParts = currentTuple.split('_');
                        if (currentParts.size() == 3) {
                            QString currentIpPart = currentParts[2];
                            int currentColonIndex = currentIpPart.indexOf(':');
                            if (currentColonIndex != -1) {
                                QString currentIp = currentIpPart.left(currentColonIndex);
                                if (currentIp == ip) {
                                    stillActive = true;
                                    qDebug() << "  IP still active in current connections";
                                    break;
                                }
                            }
                        }
                    }
                    
                    if (!stillActive && ipAccessMap.contains(ip)) {
                        IPAccessRecord& record = ipAccessMap[ip];
                        if (record.isActive) {
                            record.endTime = currentTime;
                            record.isActive = false;
                            pendingSendRecords.insert(ip, record);
                            qDebug() << "  Marked IP as ended:" << ip;
                            qDebug() << "  Start:" << record.startTime.toString("yyyy-MM-dd hh:mm:ss");
                            qDebug() << "  End:" << record.endTime.toString("yyyy-MM-dd hh:mm:ss");
                        }
                    }
                }
            }
        }
    } else {
        qDebug() << "First collection, no previous data";
    }
    
//    qDebug() << "=== Active IP Check ===";
    QSet<QString> activeIPs;
    
    QList<QString> connectionStateKeys = connectionStates.keys();
    for (int i = 0; i < connectionStateKeys.size(); i++) {
        QString key = connectionStateKeys[i];
        QString state = connectionStates[key];
        
        QStringList parts = key.split('_');
        if (parts.size() == 3) {
            QString ipPart = parts[2];
            int colonIndex = ipPart.indexOf(':');
            if (colonIndex != -1) {
                QString ip = ipPart.left(colonIndex);
                
                bool isActiveState = true;
                if (state == "CLOSED" || state == "TIME_WAIT" || state == "FIN_WAIT2" || state == "CLOSE_WAIT" || state == "LAST_ACK") {
                    isActiveState = false;
                }
                
                if (isActiveState) {
                    activeIPs.insert(ip);
//                    qDebug() << "IP remains active:" << ip;
                }
            }
        }
    }
    
//    qDebug() << "Active IPs currently:" << activeIPs.size();
    
    QList<QString> ipAccessMapKeys = ipAccessMap.keys();
    for (int i = 0; i < ipAccessMapKeys.size(); i++) {
        QString key = ipAccessMapKeys[i];
        const IPAccessRecord record = ipAccessMap[key];
        
        if (record.isActive) {
            bool isStillActive = activeIPs.contains(key);
            
            if (!isStillActive) {
                ipAccessMap[key].endTime = currentTime;
                ipAccessMap[key].isActive = false;
                
                pendingSendRecords.insert(key, ipAccessMap[key]);
//                qDebug() << "IP became inactive:" << key;
//                qDebug() << "  Start:" << ipAccessMap[key].startTime.toString("yyyy-MM-dd hh:mm:ss");
//                qDebug() << "  End:" << ipAccessMap[key].endTime.toString("yyyy-MM-dd hh:mm:ss");
            }
        }
    }
    
//    qDebug() << "=== Pending Send Records ===";
//    qDebug() << "Pending records size:" << pendingSendRecords.size();
    QList<QString> pendingKeys = pendingSendRecords.keys();
    for (int i = 0; i < pendingKeys.size(); i++) {
        QString key = pendingKeys[i];
        IPAccessRecord record = pendingSendRecords[key];
//        qDebug() << "Pending send for IP:" << key;
//        qDebug() << "  Start:" << record.startTime.toString("yyyy-MM-dd hh:mm:ss");
//        qDebug() << "  End:" << record.endTime.toString("yyyy-MM-dd hh:mm:ss");
    }
    
    QList<QString> endedIPs;
    QList<QString> ipAccessMapKeysForCleanup = ipAccessMap.keys();
    for (int i = 0; i < ipAccessMapKeysForCleanup.size(); i++) {
        QString key = ipAccessMapKeysForCleanup[i];
        const IPAccessRecord record = ipAccessMap[key];
        if (!record.isActive) {
            endedIPs.append(key);
        }
    }
    
    for (int i = 0; i < endedIPs.size(); i++) {
        QString ip = endedIPs[i];
        ipAccessMap.remove(ip);
        qDebug() << "Removed ended IP from map:" << ip;
    }
    
//    qDebug() << "=== Current State ===";
//    qDebug() << "IP Access Map size:" << ipAccessMap.size();
    QList<QString> currentMapKeys = ipAccessMap.keys();
    for (int i = 0; i < currentMapKeys.size(); i++) {
        QString key = currentMapKeys[i];
        IPAccessRecord record = ipAccessMap[key];
        qDebug() << "Active IP in map:" << key << "Start:" << record.startTime.toString("yyyy-MM-dd hh:mm:ss");
    }
    
    lastConnectionTuples = currentConnectionTuples;
//    qDebug() << "=== Network Collection End ===\n";
    
    return getIPAccessRecords();
}

QList<IPAccessRecord> networkMonitor::getIPAccessRecords()
{
    return ipAccessMap.values();
}

QList<IPAccessRecord> networkMonitor::getPendingSendRecords()
{
    return pendingSendRecords.values();
}

void networkMonitor::clearPendingSendRecords()
{
    QList<QString> keys = pendingSendRecords.keys();
    for (int i = 0; i < keys.size(); i++) {
        QString key = keys[i];
        if (ipAccessMap.contains(key)) {
            ipAccessMap.remove(key);
        }
    }
    
    qDebug() << "Cleared" << pendingSendRecords.size() << "pending send records";
    pendingSendRecords.clear();
}

QString networkMonitor::getTcpState(DWORD state)
{
    switch (state) 
    {
    case MIB_TCP_STATE_CLOSED: return "CLOSED";
    case MIB_TCP_STATE_LISTEN: return "LISTENING";
    case MIB_TCP_STATE_SYN_SENT: return "SYN_SENT";
    case MIB_TCP_STATE_SYN_RCVD: return "SYN_RECEIVED";
    case MIB_TCP_STATE_ESTAB: return "ESTABLISHED";
    case MIB_TCP_STATE_FIN_WAIT1: return "FIN_WAIT1";
    case MIB_TCP_STATE_FIN_WAIT2: return "FIN_WAIT2";
    case MIB_TCP_STATE_CLOSE_WAIT: return "CLOSE_WAIT";
    case MIB_TCP_STATE_CLOSING: return "CLOSING";
    case MIB_TCP_STATE_LAST_ACK: return "LAST_ACK";
    case MIB_TCP_STATE_TIME_WAIT: return "TIME_WAIT";
    default: return "UNKNOWN";
    }
}
