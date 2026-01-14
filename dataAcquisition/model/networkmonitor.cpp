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
            
            // 本地地址和端口
            struct in_addr localAddr;
            localAddr.S_un.S_addr = tcpTable->table[i].dwLocalAddr;
            info.localAddress = QString(inet_ntoa(localAddr));
            info.localPort = ntohs((u_short)tcpTable->table[i].dwLocalPort);
            
            // 远程地址和端口
            struct in_addr remoteAddr;
            remoteAddr.S_un.S_addr = tcpTable->table[i].dwRemoteAddr;
            info.remoteAddress = QString(inet_ntoa(remoteAddr));
            info.remotePort = ntohs((u_short)tcpTable->table[i].dwRemotePort);
            
            // 连接状态
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

