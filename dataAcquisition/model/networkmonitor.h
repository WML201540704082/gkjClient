#ifndef NETWORKMONITOR_H
#define NETWORKMONITOR_H

#include <QString>
#include <QDebug>
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


class networkMonitor
{
public:
    networkMonitor();
    
    QList<ConnectionInfo> getTcpConnections();
    QList<ConnectionInfo> getUdpConnections();

private:
    QString getTcpState(DWORD state); 
    
    
};

#endif // NETWORKMONITOR_H
