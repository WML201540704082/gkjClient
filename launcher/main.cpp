#include <QApplication>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QSystemSemaphore>
#include <QFile>
#include <QDir>
#include <QProcess>
#include <QMessageBox>
#include <QLocalSocket>
#include <QThread>
#include <QSettings>
#include <QMutex>
#include <QDateTime>

#include "control.h"
#include "utf8.h"

QString saveFilePath;//存储配置文件的路径
QString saveLogFilePath;
QString iniLogFile;//logConfig.ini文件完整路径

int logType = 4;  //默认日志输出等级

enum
{
    Fatal = 0,
    Critical,
    Warining,
    Info,
    Debug
} logLeaver;

// 全局 QProcess，用于启动和管理 client
QProcess *clientProcess = nullptr;

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
    saveFilePath = QCoreApplication::applicationDirPath() + "/config";
    saveLogFilePath = saveFilePath + "/launcherLog";
    iniLogFile = saveFilePath + "/launcherLogConfig.ini";
    
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

bool isDataAcquisitionRunning()
{
    QProcess process;
    process.start("tasklist", QStringList() << "/FI" << "IMAGENAME eq dataAcquisition.exe");
    process.waitForFinished();
    QString output = process.readAllStandardOutput();
    return output.contains("dataAcquisition.exe");
}

void startDataAcquisition()
{
    if (!isDataAcquisitionRunning())
    {
        QProcess *dataAcquisitionProcess = new QProcess();
        dataAcquisitionProcess->start(QCoreApplication::applicationDirPath() + "./dataAcquisition.exe");

        if (!dataAcquisitionProcess->waitForStarted())
        {
            QMessageBox::warning(nullptr, "错误", "无法启动客户端程序！");
            delete dataAcquisitionProcess;
            dataAcquisitionProcess = nullptr;
        }
    }
}

///
/// \brief isClientRunning 检查 client 是否已经运行
/// \return 
///
bool isClientRunning()
{
    // 如果 clientProcess 非空，且未结束，表示 client 正在运行
//    return clientProcess && clientProcess->state() == QProcess::Running;
    
    
    QProcess process;
    process.start("tasklist", QStringList() << "/FI" << "IMAGENAME eq client.exe");
    process.waitForFinished();
    QString output = process.readAllStandardOutput();
    return output.contains("client.exe");
}

///
/// \brief startClient 启动 client
///
void startClient(const QString &arg )
{
    if (!isClientRunning())
    {
        clientProcess = new QProcess();
        
        QStringList arguments;
        if (!arg.isEmpty())
        {
            arguments << arg;  // 传递参数
        }
        clientProcess->start(QCoreApplication::applicationDirPath() + "./client.exe", arguments);
//        clientProcess->start("./client.exe"); 
        if (!clientProcess->waitForStarted())
        {
            QMessageBox::warning(nullptr, "错误", "无法启动客户端程序！");
            delete clientProcess;
            clientProcess = nullptr;
        }
    }
}

bool isProcessRunning(qint64 pid)
{
    QProcess process;
    process.start("tasklist", QStringList() << "/FI" << QString("PID eq %1").arg(pid));
    process.waitForFinished();
    QString output = process.readAllStandardOutput();
    return output.contains(QString::number(pid));
}

void isRepeatStart()
{
    //定义锁文件路径
    const QString lockFilePath = QDir::tempPath() + "/frlauncher.lock";
    
    //信号量名称
    const QString semaphoreKey = "launcherSemaphore";   
    
    //创建一个系统信号量
    QSystemSemaphore semaphore(semaphoreKey, 1);
    semaphore.acquire();
    
    QFile lockFile(lockFilePath);
    bool isRuning = false;
    
    //尝试创建锁文件
    if(lockFile.exists())
    {
        if(lockFile.open(QIODevice::ReadOnly))
        {
            QByteArray pidData = lockFile.readAll();
            lockFile.close();
            bool ok;
            qint64 pid = pidData.toLongLong(&ok);
            if(ok && isProcessRunning(pid))
            {
                isRuning = true;
            }
            else
            {
                lockFile.remove();
            }
        }
    }
    
    if(!isRuning)
    {
        if(lockFile.open(QIODevice::WriteOnly))
        {
            lockFile.write(QByteArray::number(QCoreApplication::applicationPid()));
            lockFile.close();
        }
        else
        {
            isRuning = true;
        }
    }
    
    semaphore.release();
    
    if(isRuning)
    {
        qDebug()<<"检测到重复启动，程序即将退出";
        QMessageBox::warning(0, "重复启动", "已有人脸识别程序正在运行，请勿重复启动");
        exit(0);
    }
}

///
/// \brief setAutoRun 设置或取消自启动
/// \param enable
///
void setAutoRun(bool enable)
{
    QString appName = "frLauncher"; // 程序名称
    QString appPath = "\"" + QCoreApplication::applicationFilePath().replace("/", "\\") + "\" --autorun";

    QSettings reg("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);

    if (enable)
    {
        reg.setValue(appName, appPath);  // 设置自启动
        qDebug() << "已启用自启动：" << appPath;
    } 
    else 
    {
        reg.remove(appName);  // 取消自启动
        qDebug() << "已禁用自启动";
    }
}


///
/// \brief isAutoRunEnabled 检查当前是否已启用自启动
/// \return 
///
bool isAutoRunEnabled()
{
    QString appName = "frLauncher";
    QSettings reg("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    return reg.contains(appName);  // 如果存在该键，说明已启用自启动
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    logFileInit();
    qInstallMessageHandler(setMsgOutput);
    cleanOldLogs(saveLogFilePath, 3);
    
    //防止重复启动
    isRepeatStart();   
    
    //启动采集程序
    startDataAcquisition();
    
    //判断launcher是以自启动方式打开还是手动打开
    QStringList arguments = QCoreApplication::arguments();
    if(!arguments.contains("--autorun"))
    {
        //手动打开时判断client启动状态，未启动则将client启动
        if(!isClientRunning())
        {
            startClient("");
        }
        //判断是否开启了开机自启，没有则开启
        if (!isAutoRunEnabled())
        {
            setAutoRun(true);
            qDebug() << "已自动开启自启动";
        }
    }   
    
    //先设置系统托盘
    QMenu sysTrayMenu;
    QSystemTrayIcon mySysTrayIcon(QIcon(":/images/images/logo3@2x.ico"), &a);
    mySysTrayIcon.setToolTip("人脸识别客户端");
    mySysTrayIcon.show();
    
    
    QAction *pAdminRegister;
    QAction *pTmpAuthorize;
//    QAction *pSetConfPwd;
    QAction *pClose;
    
    pAdminRegister = new QAction("管理员注册", &sysTrayMenu);
    QObject::connect(pAdminRegister, &QAction::triggered, [&](){
        if (!isClientRunning()) {
            startClient("showAdminRegister");
            QThread::msleep(1000); // 等待client初始化（根据实际情况调整）
        }
        
        QLocalSocket socket;
        socket.connectToServer("ClientServer");
        if (socket.waitForConnected(1000)) { // 1秒超时
            socket.write("showAdminRegister");
            socket.waitForBytesWritten();
        } else {
            QMessageBox::warning(0, "错误", "无法连接到客户端");
        }
    });
    
    pTmpAuthorize = new QAction("临时授权管理", &sysTrayMenu);
    QObject::connect(pTmpAuthorize, &QAction::triggered, [&](){
        if (!isClientRunning()) {
            startClient("showTmpAuthorize");
            QThread::msleep(1000); // 等待client初始化（根据实际情况调整）
        }
        
        QLocalSocket socket;
        socket.connectToServer("ClientServer");
        if (socket.waitForConnected(1000)) { // 1秒超时
            socket.write("showTmpAuthorize");
            socket.waitForBytesWritten();
        } else {
            QMessageBox::warning(0, "错误", "无法连接到客户端");
        }
    });
    
//    pSetConfPwd = new QAction("配置密码验证", &sysTrayMenu);
//    QObject::connect(pSetConfPwd, &QAction::triggered, [&](){
//        if (!isClientRunning()) {
//            startClient("showConfPwdConfigure");
//            QThread::msleep(1000); // 等待client初始化（根据实际情况调整）
//        }
        
//        QLocalSocket socket;
//        socket.connectToServer("ClientServer");
//        if (socket.waitForConnected(1000)) { // 1秒超时
//            socket.write("showConfPwdConfigure");
//            socket.waitForBytesWritten();
//        } else {
//            QMessageBox::warning(0, "错误", "无法连接到客户端");
//        }
//    });
    
    pClose = new QAction("关闭程序", &sysTrayMenu);
    QObject::connect(pClose, &QAction::triggered, [&](){
        if (isClientRunning()) {
            QLocalSocket socket;
            socket.connectToServer("ClientServer");
            if (socket.waitForConnected(1000)) {
                socket.write("shutdown");
                socket.waitForBytesWritten();
            }
            clientProcess->waitForFinished(); // 等待进程结束
        }
        QApplication::quit();
    });
    
    // 将 QAction 添加到菜单
    sysTrayMenu.addAction(pAdminRegister);
    sysTrayMenu.addAction(pTmpAuthorize);
//    sysTrayMenu.addAction(pSetConfPwd);
    sysTrayMenu.addAction(pClose);
//    sysTrayMenu.addAction("关闭程序", &sysTrayMenu, &QApplication::quit);
    
    mySysTrayIcon.setContextMenu(&sysTrayMenu);
    
    QObject::connect(&mySysTrayIcon, &QSystemTrayIcon::activated, [&](QSystemTrayIcon::ActivationReason reason)
    {
        switch (reason)
        {
        case QSystemTrayIcon::DoubleClick:
            if(isClientRunning())
            {
                QLocalSocket socket;
                socket.connectToServer("ClientServer");
                if (socket.waitForConnected(1000)) { // 1秒超时
                    socket.write("showCurrent");
                    socket.waitForBytesWritten();
                } else {
                    QMessageBox::warning(0, "错误", "无法连接到客户端");
                }
            }
            else
            {                
                startClient("");
            }
            
            break;
            
        default:
            break;
        }
    });
            
    control *pControl = new control();
    
//    MainWindow w;
//    w.show();
    return a.exec();
}
