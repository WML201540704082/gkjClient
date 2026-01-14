#include "mainwindow.h"

#include <QApplication>
#include <QDir>
#include <QMutex>
#include <QSettings>
#include <QNetworkInterface>

QString saveFilePath;//存储配置文件的路径
QString saveLogFilePath;
QString iniLogFile;//logConfig.ini文件完整路径
QString fingerprint;
volatile qint64 timeDifference = 0; //本机和服务器的时间差
//QString urlCommon = "http://172.20.10.3:18084/idevelop-ipc/face";
QString urlCommon = "http://25.41.34.27/api/idevelop-ipc/face";
HANDLE m_Handle;
float similarityThreshold = 85.0;//人脸相似度阈值

int logType = 4;  //默认日志输出等级

enum
{
    Fatal = 0,
    Critical,
    Warining,
    Info,
    Debug
} logLeaver;

double w_fx = 0; // 横向缩放因子
double h_fx = 0; // 纵向缩放因子

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
    saveLogFilePath = saveFilePath + "/clientLog";
    iniLogFile = saveFilePath + "/clientLogConfig.ini";
    
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

int main(int argc, char *argv[])
{    
    QApplication a(argc, argv);
    
    logFileInit();
    qInstallMessageHandler(setMsgOutput);
    cleanOldLogs(saveLogFilePath, 3);
    iniFingerprint();
    
    MainWindow w;
//    // 处理命令行参数
//    QStringList arguments = QCoreApplication::arguments();
//    if (arguments.contains("showAdminRegister"))
//    {
//        w.setVisible(false);
//    }
//    else if (arguments.contains("showCurrent"))
//    {
//        w.setVisible(false);
//    }
//    else
//    {
//        w.setVisible(true);
//    }
    
    return a.exec();
}
