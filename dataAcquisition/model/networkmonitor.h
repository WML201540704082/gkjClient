#ifndef NETWORKMONITOR_H
#define NETWORKMONITOR_H

#include <QString>
#include <QDebug>
#include <QDateTime>
#include <QMap>
#include <QSet>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h> 

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

struct ConnectionInfo {
    QString protocol;
    QString localAddress;
    quint16 localPort;
    QString remoteAddress;
    quint16 remotePort;
    QString state;
    DWORD processId;
};

struct IPAccessRecord {
    QString ip;
    QDateTime startTime;
    QDateTime endTime;
    bool isActive;
};

class networkMonitor {
public:
    networkMonitor();
    
    QList<ConnectionInfo> getTcpConnections();
    QList<ConnectionInfo> getUdpConnections();
    
    QList<ConnectionInfo> getAllConnections();
    
    QList<IPAccessRecord> updateIPAccessRecords();
    
    QList<IPAccessRecord> getIPAccessRecords();
    
    QList<IPAccessRecord> getPendingSendRecords();
    
    void clearPendingSendRecords();

private:
    QString getTcpState(DWORD state); 
    
    bool isInternalIP(const QString &ip);
    
    QString generateTupleKey(const ConnectionInfo &conn);
    
    QSet<QString> lastConnectionTuples;
    
    QMap<QString, IPAccessRecord> ipAccessMap;
    
    QMap<QString, IPAccessRecord> pendingSendRecords;
};

#endif // NETWORKMONITOR_H
