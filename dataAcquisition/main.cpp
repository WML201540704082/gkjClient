#include "model/networkmonitor.h"
#include "control.h"
#include "utf8.h"

#include <QApplication>
#include <QDebug>
#include <QDateTime>
#include <windows.h>
#include <QAxObject>
#include <QDir>
#include <QSettings>
#include <QMutex>
#include <QNetworkInterface>

QString saveFilePath;//存储配置文件的路径
QString saveLogFilePath;
QString iniLogFile;//logConfig.ini文件完整路径
QString fingerprint;
volatile qint64 timeDifference = 0; //本机和服务器的时间差
//QString urlCommon = "http://172.20.10.3:18084/idevelop-ipc/face";
//QString urlCommon = "http://25.41.34.27/api/idevelop-ipc/face";
QString urlCommon = "http://172.20.10.12:18084/idevelop-ipc/face";

int logType = 4;  //默认日志输出等级

enum
{
    Fatal = 0,
    Critical,
    Warining,
    Info,
    Debug
} logLeaver;

bool createPath(const QString& path)
{
    //qDebug() << path;
    QDir dir;
    if (!dir.exists(path))
    {
        return dir.mkpath(path);
    }
    return false;
}

void logFileInit()
{
    saveFilePath =QCoreApplication::applicationDirPath() + "/config";
    saveLogFilePath = saveFilePath + "/dataAcquisitionLog";
    iniLogFile = saveFilePath + "/dataAcquisitionLogConfig.ini";
    
    createPath(saveFilePath);
    createPath(saveLogFilePath);

    QFile file(iniLogFile);

    QSettings* pSetting;
    if (!file.exists())
    {
        pSetting = new QSettings(iniLogFile, QSettings::IniFormat);
        pSetting->setValue("LogConfig/level", "4");
    }
    pSetting = new QSettings(iniLogFile, QSettings::IniFormat);
    logType = pSetting->value("LogConfig/level").toInt();

    delete pSetting;
    pSetting = nullptr;
}

void setMsgOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    //避免同时读写文件
    static QMutex mutex;
    mutex.lock();

    QByteArray localMsg = msg.toUtf8();
    QString strOutput = "";
    switch (type)
    {
    case QtDebugMsg:
        if (logType == Debug)
        {
            strOutput = QString("%1 %2 %3 %4 [Debug] %5 \n")
                        .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                        .arg(context.file)
                        .arg(context.line)
                        .arg(QString(context.function))
                        .arg(QString(localMsg));
        }
        break;

    case QtInfoMsg:
        if (logType >= Info)
        {
            strOutput = QString("%1 %2 %3 %4 [Info] %5 \n")
                        .arg(QDateTime::currentDateTime().toString(
                                 "yyyy-MM-dd hh:mm:ss"))
                        .arg(context.file)
                        .arg(context.line)
                        .arg(QString(context.function))
                        .arg(QString(localMsg));
        }
        break;

    case QtWarningMsg:
        if (logType >= Warining)
        {
            strOutput = QString("%1 %2 %3 %4 [Warning] %5 \n")
                        .arg(QDateTime::currentDateTime().toString(
                                 "yyyy-MM-dd hh:mm:ss"))
                        .arg(context.file)
                        .arg(context.line)
                        .arg(QString(context.function))
                        .arg(QString(localMsg));
        }
        break;

    case QtCriticalMsg:
        if (logType >= Critical)
        {
            strOutput = QString("%1 %2 %3 %4 [Critical] %5 \n")
                        .arg(QDateTime::currentDateTime().toString(
                                 "yyyy-MM-dd hh:mm:ss"))
                        .arg(context.file)
                        .arg(context.line)
                        .arg(QString(context.function))
                        .arg(QString(localMsg));
        }
        break;

    case QtFatalMsg:
        if (logType >= Fatal)
        {
            strOutput = QString("%1 %2 %3 %4 [Fatal] %5 \n")
                        .arg(QDateTime::currentDateTime().toString(
                                 "yyyy-MM-dd hh:mm:ss"))
                        .arg(context.file)
                        .arg(context.line)
                        .arg(QString(context.function))
                        .arg(QString(localMsg));
        }
        abort();
    }
    //每天生成一个新的log文件
    QString fileName = QString("%1/%2.log").arg(saveLogFilePath).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd"));
    QFile logFile(fileName);
    logFile.open(QIODevice::WriteOnly | QIODevice::Append);
    if (strOutput != "")
    {
        QTextStream logStream(& logFile);
        logStream << strOutput << "\r\n";
        QTextStream(stdout) << strOutput << "\r\n";
    }
    //清除缓存文件，解锁
    logFile.flush();
    logFile.close();
    mutex.unlock();
}

void cleanOldLogs(const QString &logDirPath, int months)
{
    QDir logDir = logDirPath;
    qWarning()<<"logDirPath: "<<logDirPath;
    if(!logDir.exists())
    {
        qWarning()<<"!logDir.exists()";
        return;
    }
    
    //获取当前日期months个月的日期
    QDate monthsAgo = QDate::currentDate().addMonths(-months);
    //获取所有log文件
    QStringList logFileList = logDir.entryList(QStringList() << "*.log", 
                                               QDir::Files, QDir::Name);
    
    for(const QString &fileName : logFileList)
    {
        QString datePart = fileName.left(10);
        QDate fileDate = QDate::fromString(datePart, "yyyy-MM-dd");
        if(fileDate.isValid() && fileDate < monthsAgo)
        {
            //超过months个月，删除
            qWarning()<<"fileDate: "<<fileDate;
            QString filePath = logDir.absoluteFilePath(fileName);
            qWarning()<<"filePath: "<<filePath;
            QFile::remove(filePath);
        }
    }
}

QPair<QString, QString> getIpMac()
{
    const auto list = QNetworkInterface::allInterfaces();
    for (const auto& l : list)
    {
        if (l.flags().testFlag(QNetworkInterface::IsUp)
            && l.flags().testFlag(QNetworkInterface::IsRunning)
            && !l.flags().testFlag(QNetworkInterface::IsLoopBack))
        {
            if (l.addressEntries().size() > 0)
            {
                for(auto const addressEntry : l.addressEntries())
                {
                    if(addressEntry.ip().protocol() == QAbstractSocket::IPv4Protocol)
                    {
                        qDebug()<<"addressEntry.ip().toString() "<<addressEntry.ip().toString();
                        return QPair<QString, QString>(addressEntry.ip().toString(), l.hardwareAddress());
                    }
                }    
            }
            return QPair<QString, QString>("", l.hardwareAddress());
        }
    }
    return QPair<QString, QString>();
}

void iniFingerprint()
{
    const auto ipMac =getIpMac();
    QString ip = ipMac.first;

    if(ip.isEmpty())
    {
        return;
    }
    fingerprint = ip;
}

//void printSystemBootTime() 
//{
//    // 1. 获取系统启动后的毫秒数 (Uptime)
//    // GetTickCount64 返回的是 unsigned long long 类型
//    qint64 uptimeMsecs = static_cast<qint64>(GetTickCount64());

//    // 2. 获取当前时间
//    QDateTime now = QDateTime::currentDateTime();

//    // 3. 计算开机时间：当前时间 减去 运行时长
//    // addMSecs 传入负数即为向前推算
//    QDateTime bootTime = now.addMSecs(-uptimeMsecs);

//    // 4. 输出结果
//    qDebug() << "系统已运行时间(毫秒):" << uptimeMsecs;
//    qDebug() << "系统已运行时间(格式化):" 
//             << (uptimeMsecs / 1000 / 60 / 60) << "小时" 
//             << ((uptimeMsecs / 1000 / 60) % 60) << "分钟";
//    qDebug() << "推算的开机时间:" << bootTime.toString("yyyy-MM-dd HH:mm:ss");
//}

//QDateTime parseWMIDateTime(const QString &wmiTime)
//{
//    if (wmiTime.isEmpty() || wmiTime.length() < 14)
//    {
//        return QDateTime();
//    }
    
//    int year = wmiTime.mid(0, 4).toInt();
//    int month = wmiTime.mid(4, 2).toInt();
//    int day = wmiTime.mid(6, 2).toInt();
//    int hour = wmiTime.mid(8, 2).toInt();
//    int minute = wmiTime.mid(10, 2).toInt();
//    int second = wmiTime.mid(12, 2).toInt();
    
//    QDateTime dateTime(QDate(year, month, day), QTime(hour, minute, second));
//    return dateTime;
//}

//QDateTime getBootTimeViaWMI()
//{
//    // 初始化COM
//    CoInitialize(NULL);
    
//    QAxObject *wmi = new QAxObject("WbemScripting.SWbemLocator");
    
//    if (wmi->isNull())
//    {
//        qDebug() << "无法创建WMI对象";
//        delete wmi;
//        CoUninitialize();
//        return QDateTime();
//    }
    
//    QAxObject *service = wmi->querySubObject(
//        "ConnectServer(QString, QString)", 
//        ".", "root\\cimv2");
    
//    if (!service || service->isNull()) 
//    {
//        qDebug() << "无法连接到WMI服务";
//        delete wmi;
//        CoUninitialize();
//        return QDateTime();
//    }
    
//    QAxObject *results = service->querySubObject(
//        "ExecQuery(QString)", 
//        "SELECT * FROM Win32_OperatingSystem");  // 使用 SELECT *
    
//    if (!results || results->isNull())
//    {
//        qDebug() << "WMI查询失败";
//        delete service;
//        delete wmi;
//        CoUninitialize();
//        return QDateTime();
//    }
    
//    int count = results->property("Count").toInt();
//    qDebug() << "查询结果数量:" << count;
    
//    QDateTime bootTime;
    
//    if (count > 0)
//    {
//        QAxObject *item = results->querySubObject("ItemIndex(int)", 0);
        
//        if (item && !item->isNull())
//        {
//            // 方法1: 使用 dynamicCall 获取属性
//            QVariant value = item->dynamicCall("LastBootUpTime()");
//            QString wmiTimeStr = value.toString();
            
//            qDebug() << "方法1 - 原始WMI时间字符串:" << wmiTimeStr;
            
//            // 如果方法1失败，尝试方法2
//            if (wmiTimeStr.isEmpty())
//            {
//                // 方法2: 直接获取属性值
//                QAxObject *prop = item->querySubObject("Properties_(QString)", "LastBootUpTime");
//                if (prop && !prop->isNull())
//                {
//                    wmiTimeStr = prop->property("Value").toString();
//                    qDebug() << "方法2 - 原始WMI时间字符串:" << wmiTimeStr;
//                    delete prop;
//                }
//            }
            
//            if (!wmiTimeStr.isEmpty()) 
//            {
//                bootTime = parseWMIDateTime(wmiTimeStr);
//            }
            
//            delete item;
//        }
//    }
    
//    delete results;
//    delete service;
//    delete wmi;
//    CoUninitialize();
    
//    return bootTime;
//}

//// 使用示例
//void collectBootTime()
//{
//    bootTime timeObj;
//    QDateTime bootTime = timeObj.getBootTimeViaWMI();
    
//    if (bootTime.isValid())
//    {
//        qDebug() << "系统开机时间:" << bootTime.toString("yyyy-MM-dd hh:mm:ss");
        
//        qint64 uptime = bootTime.secsTo(QDateTime::currentDateTime());
//        qDebug() << "系统已运行:" << uptime / 3600 << "小时" << (uptime % 3600) / 60 << "分钟";
    
//    } 
//    else 
//    {
//        qDebug() << "获取开机时间失败";
//    }
//}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    logFileInit();
    qInstallMessageHandler(setMsgOutput);
    cleanOldLogs(saveLogFilePath, 3);
    iniFingerprint();
    
//    networkMonitor monitor;
    
//    // 获取TCP连接
//    auto tcpConns = monitor.getTcpConnections();
//    for (const auto& conn : tcpConns) {
//        qDebug() << conn.protocol 
//                 << conn.localAddress << ":" << conn.localPort
//                 << "->"
//                 << conn.remoteAddress << ":" << conn.remotePort
//                 << conn.state;
//    }
    
//    // 获取UDP连接
//    auto udpConns = monitor.getUdpConnections();
//    for (const auto& conn : udpConns) {
//        qDebug() << conn.protocol 
//                 << conn.localAddress << ":" << conn.localPort
//                 << "->"
//                 << conn.remoteAddress << ":" << conn.remotePort
//                 << conn.state;
//    }
    
//    printSystemBootTime();
    
    control *pControl = new control();
    
    return a.exec();
}
