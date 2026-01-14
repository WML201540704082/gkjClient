#include "mainwindow.h"

#pragma execution_character_set("utf-8")

#include <QApplication>
#include <QDateTime>
#include <QDebug>
#include <windows.h>

QDateTime getBootTime_Method1() {
    ULONGLONG tickCount = GetTickCount64();
    QDateTime bootTime = QDateTime::currentDateTime()
                         .addMSecs(-static_cast<qint64>(tickCount));
    return bootTime;
}

// 方法2: 使用性能计数器 (更精确)
QDateTime getBootTime_Method2() {
    FILETIME ft;
    ULARGE_INTEGER uli;
    
    GetSystemTimeAsFileTime(&ft);
    uli.LowPart = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;
    
    ULONGLONG currentTime = uli.QuadPart;
    ULONGLONG tickCount = GetTickCount64() * 10000; // 转换为100纳秒单位
    
    uli.QuadPart = currentTime - tickCount;
    ft.dwLowDateTime = uli.LowPart;
    ft.dwHighDateTime = uli.HighPart;
    
    SYSTEMTIME st;
    FileTimeToSystemTime(&ft, &st);
    
    QDateTime bootTime(QDate(st.wYear, st.wMonth, st.wDay),
                       QTime(st.wHour, st.wMinute, st.wSecond));
    
    return bootTime;
}

// 完整测试
void testBootTime() {
    qDebug() << "=== 方法1 (GetTickCount64) ===";
    QDateTime bootTime1 = getBootTime_Method1();
    qDebug() << "开机时间:" << bootTime1.toString("yyyy-MM-dd hh:mm:ss");
    
    qDebug() << "\n=== 方法2 (性能计数器) ===";
    QDateTime bootTime2 = getBootTime_Method2();
    qDebug() << "开机时间:" << bootTime2.toString("yyyy-MM-dd hh:mm:ss");
    
//    qDebug() << "\n=== 方法3 (WMI) ===";
//    QDateTime bootTime3 = getBootTimeViaWMI();
//    if (bootTime3.isValid()) {
//        qDebug() << "开机时间:" << bootTime3.toString("yyyy-MM-dd hh:mm:ss");
//    }
    
    // 计算运行时长
    qint64 uptime = bootTime1.secsTo(QDateTime::currentDateTime());
    int days = uptime / 86400;
    int hours = (uptime % 86400) / 3600;
    int minutes = (uptime % 3600) / 60;
    
    qDebug() << "\n系统已运行:" << days << "天" << hours << "小时" << minutes << "分钟";
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    
    qDebug()<<"===============================";
    testBootTime();
    
    return a.exec();
}
