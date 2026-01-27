#ifndef MYSTRUCT_H
#define MYSTRUCT_H

#include <QString>

struct powerOnOffTimeInfo
{
    QString powerOnTime;
    QString powerOffTime;
};

struct networkInfo
{
    QString protocol;
    QString localAddr;
    QString localPort;
    QString remoteAddr;
    QString remotePort;
    QString status;
};

#endif // MYSTRUCT_H
