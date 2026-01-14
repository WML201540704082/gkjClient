#include "boottime.h"
#include "utf8.h"

#include <QDateTime>
#include <QDebug>
#include <QDateTime>
#include <windows.h>
#include <QAxObject>

bootTime::bootTime(QObject *parent) : QObject(parent)
{
    
}

QDateTime bootTime::parseWMIDateTime(const QString &wmiTime)
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

QDateTime bootTime::getBootTimeViaWMI()
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
