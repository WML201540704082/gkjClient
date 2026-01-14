#include "screenstatuscheck.h"
#include "utf8.h"

#include <QTimer>
#include <windows.h>
#include <wtsapi32.h>
#include <QCoreApplication>
#include <QDebug>

#pragma comment(lib, "Wtsapi32.lib")

screenStatusCheck::screenStatusCheck(QObject *parent) : QObject(parent)
{
    
}

bool screenStatusCheck::isScreenLocked()
{
    return GetForegroundWindow() == nullptr; // 如果没有前景窗口则系统锁定
}

void screenStatusCheck::onCheckStart()
{
    // 定时检查锁屏状态
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &screenStatusCheck::checkLockStatus);
    timer->start(1000); // 每秒检查一次
}

void screenStatusCheck::checkLockStatus()
{
    //    // 获取当前会话的状态
    //    WTS_SESSION_INFO *sessionInfo;
    //    DWORD count;
    
    //    HANDLE hServer = WTSOpenServer(const_cast<wchar_t*>(L"localhost"));
    //    if (hServer == nullptr)
    //        return;
    
    //    if (WTSEnumerateSessions(hServer, 0, 1, &sessionInfo, &count))
    //    {
    //        for (DWORD i = 0; i < count; i++)
    //        {
    //            if (sessionInfo[i].State == WTSActive)
    //            {
    //                // 用户已经锁屏
    //                if (isScreenLocked())
    //                {
    //                    qDebug()<<"screen locked！检测到锁屏,退出程序";
    //                    qApp->quit();  // 锁屏后退出程序
    //                }
    //            }
    //        }
    //    }
    
    
    HDESK hDesk = OpenInputDesktop(0, FALSE, DESKTOP_READOBJECTS);
    if (hDesk == nullptr)
    {
        qDebug() << "无法打开桌面，可能已锁屏";
        emit logout();
        qDebug()<<"emit logout()";
        qApp->quit();
        return; // 这里没有句柄，不需要关闭
    }
    
    bool isLocked = false;
    DWORD len;
    char name[256];
    
    if (GetUserObjectInformation(hDesk, UOI_NAME, name, 256, &len))
    {
        QString desktopName = QString::fromLocal8Bit(name);
        if (desktopName.toLower().contains("winlogon"))
        {
            qDebug() << "检测到锁屏";
            isLocked = true;
        }
    }
    
    CloseDesktop(hDesk); // 确保释放资源
    
    if (isLocked)
    {
        emit logout();
        qApp->quit();
    }
}
