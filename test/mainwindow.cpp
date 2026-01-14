#include "mainwindow.h"
#include "ui_mainwindow.h"

#pragma execution_character_set("utf-8")

#include <QApplication>
#include <QDebug>
#include <QDateTime>
#include <windows.h>
#include <QAxObject>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    QDateTime bootTime = getBootTimeViaWMI();
    
    if (bootTime.isValid())
    {
        qDebug() << "系统开机时间:" << bootTime.toString("yyyy-MM-dd hh:mm:ss");
        ui->label->setText("系统开机时间:" + bootTime.toString("yyyy-MM-dd hh:mm:ss"));
        
        qint64 uptime = bootTime.secsTo(QDateTime::currentDateTime());
        qDebug() << "系统已运行:" << uptime / 3600 << "小时" << (uptime % 3600) / 60 << "分钟";
        ui->label_2->setText(QString("系统已运行: %1 小时 %2 分钟").arg(uptime / 3600).arg((uptime % 3600) / 60));
    } 
    else 
    {
        qDebug() << "获取开机时间失败";
        ui->label_3->setText("获取开机时间失败");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::printSystemBootTime()
{
    // 1. 获取系统启动后的毫秒数 (Uptime)
    // GetTickCount64 返回的是 unsigned long long 类型
    qint64 uptimeMsecs = static_cast<qint64>(GetTickCount64());

    // 2. 获取当前时间
    QDateTime now = QDateTime::currentDateTime();

    // 3. 计算开机时间：当前时间 减去 运行时长
    // addMSecs 传入负数即为向前推算
    QDateTime bootTime = now.addMSecs(-uptimeMsecs);

    // 4. 输出结果
    qDebug() << "系统已运行时间(毫秒):" << uptimeMsecs;
    qDebug() << "系统已运行时间(格式化):" 
             << (uptimeMsecs / 1000 / 60 / 60) << "小时" 
             << ((uptimeMsecs / 1000 / 60) % 60) << "分钟";
    qDebug() << "推算的开机时间:" << bootTime.toString("yyyy-MM-dd HH:mm:ss");
}

QDateTime MainWindow::parseWMIDateTime(const QString &wmiTime)
{
    if (wmiTime.isEmpty() || wmiTime.length() < 14)
    {
        return QDateTime();
    }
    
    int year = wmiTime.mid(0, 4).toInt();
    int month = wmiTime.mid(4, 2).toInt();
    int day = wmiTime.mid(6, 2).toInt();
    int hour = wmiTime.mid(8, 2).toInt();
    int minute = wmiTime.mid(10, 2).toInt();
    int second = wmiTime.mid(12, 2).toInt();
    
    QDateTime dateTime(QDate(year, month, day), QTime(hour, minute, second));
    return dateTime;
}

QDateTime MainWindow::getBootTimeViaWMI()
{
    // 初始化COM
    CoInitialize(NULL);
    
    QAxObject *wmi = new QAxObject("WbemScripting.SWbemLocator");
    
    if (wmi->isNull())
    {
        qDebug() << "无法创建WMI对象";
        delete wmi;
        CoUninitialize();
        return QDateTime();
    }
    
    QAxObject *service = wmi->querySubObject(
        "ConnectServer(QString, QString)", 
        ".", "root\\cimv2");
    
    if (!service || service->isNull()) 
    {
        qDebug() << "无法连接到WMI服务";
        delete wmi;
        CoUninitialize();
        return QDateTime();
    }
    
    QAxObject *results = service->querySubObject(
        "ExecQuery(QString)", 
        "SELECT * FROM Win32_OperatingSystem");  // 使用 SELECT *
    
    if (!results || results->isNull())
    {
        qDebug() << "WMI查询失败";
        delete service;
        delete wmi;
        CoUninitialize();
        return QDateTime();
    }
    
    int count = results->property("Count").toInt();
    qDebug() << "查询结果数量:" << count;
    
    QDateTime bootTime;
    
    if (count > 0)
    {
        QAxObject *item = results->querySubObject("ItemIndex(int)", 0);
        
        if (item && !item->isNull())
        {
            // 方法1: 使用 dynamicCall 获取属性
            QVariant value = item->dynamicCall("LastBootUpTime()");
            QString wmiTimeStr = value.toString();
            
            qDebug() << "方法1 - 原始WMI时间字符串:" << wmiTimeStr;
            
            // 如果方法1失败，尝试方法2
            if (wmiTimeStr.isEmpty())
            {
                // 方法2: 直接获取属性值
                QAxObject *prop = item->querySubObject("Properties_(QString)", "LastBootUpTime");
                if (prop && !prop->isNull())
                {
                    wmiTimeStr = prop->property("Value").toString();
                    qDebug() << "方法2 - 原始WMI时间字符串:" << wmiTimeStr;
                    delete prop;
                }
            }
            
            if (!wmiTimeStr.isEmpty()) 
            {
                bootTime = parseWMIDateTime(wmiTimeStr);
            }
            
            delete item;
        }
    }
    
    delete results;
    delete service;
    delete wmi;
    CoUninitialize();
    
    return bootTime;
}

