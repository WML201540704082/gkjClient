//#include "screenstatuscheck.h"
//#include "utf8.h"

//#include <QTimer>
//#include <QDebug>
//#include <QCoreApplication>
//#include <QThread>
//#include <windows.h>

//screenStatusCheck::screenStatusCheck(QObject *parent) 
//    : QObject(parent)
//{
//    qDebug() << "[screenStatusCheck] 构造函数，线程ID:" << QThread::currentThreadId();
//}

//bool screenStatusCheck::isScreenLocked()
//{
//    return GetForegroundWindow() == nullptr;
//}

//void screenStatusCheck::onCheckStart()
//{
//    qDebug() << "[screenStatusCheck] 开始监控锁屏，线程ID:" << QThread::currentThreadId();
    
//    // 定时检查锁屏状态
//    QTimer *timer = new QTimer(this);
//    connect(timer, &QTimer::timeout, this, &screenStatusCheck::checkLockStatus);
//    timer->start(1000); // 每秒检查一次
//}

//void screenStatusCheck::checkLockStatus()
//{
//    HDESK hDesk = OpenInputDesktop(0, FALSE, DESKTOP_READOBJECTS);
//    if (hDesk == nullptr)
//    {
//        qDebug() << "[screenStatusCheck] 无法打开桌面，可能已锁屏";
//        performLogout();
        
//        // 使用信号通知主线程退出（更安全）
////        QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
//        return;
//    }
    
//    bool isLocked = false;
//    DWORD len;
//    char name[256];
    
//    if (GetUserObjectInformation(hDesk, UOI_NAME, name, 256, &len))
//    {
//        QString desktopName = QString::fromLocal8Bit(name);
//        if (desktopName.toLower().contains("winlogon"))
//        {
//            qDebug() << "[screenStatusCheck] 检测到锁屏桌面:" << desktopName;
//            isLocked = true;
//        }
//    }
    
//    CloseDesktop(hDesk);
    
//    if (isLocked)
//    {
//        performLogout();
////        QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
//    }
//}

//void screenStatusCheck::onSystemShutdown()
//{
//    qDebug() << "[screenStatusCheck] 收到系统关机通知，线程ID:" << QThread::currentThreadId();
//    performLogout();
//}

//void screenStatusCheck::performLogout()
//{    
//    qDebug() << "[screenStatusCheck] === 开始执行登出操作 ===";
    
//    // 发送登出信号
//    emit logout();
    
//    // 在当前线程处理事件
//    QCoreApplication::processEvents();
    
//    // 给足够时间完成数据库操作
//    QThread::msleep(300);
    
//    qDebug() << "[screenStatusCheck] === 登出操作完成 ===";
//}

#include "screenstatuscheck.h"
#include "utf8.h"

#include <QDebug>
#include <QCoreApplication>
#include <QThread>
#include <windows.h>

screenStatusCheck::screenStatusCheck(QObject *parent) 
    : QObject(parent)
    , m_checkTimer(nullptr)
    , m_startupDelayTimer(nullptr)
    , m_state(State::WaitingForStartup)
    , m_stateConfirmCount(0)
{
    qDebug() << "[screenStatusCheck] 构造函数，线程ID:" << QThread::currentThreadId();
}

QString screenStatusCheck::getDesktopName()
{
    QString desktopName = "unknown";
    
    HDESK hDesk = OpenInputDesktop(0, FALSE, DESKTOP_READOBJECTS);
    if (hDesk != nullptr)
    {
        DWORD len;
        char name[256] = {0};
        
        if (GetUserObjectInformationA(hDesk, UOI_NAME, name, 256, &len))
        {
            desktopName = QString::fromLocal8Bit(name);
        }
        CloseDesktop(hDesk);
    }
    
    return desktopName;
}

bool screenStatusCheck::isScreenLocked()
{
    HDESK hDesk = OpenInputDesktop(0, FALSE, DESKTOP_READOBJECTS);
    if (hDesk == nullptr)
    {
        return true;
    }
    
    bool isLocked = true;
    DWORD len;
    char name[256] = {0};
    
    if (GetUserObjectInformationA(hDesk, UOI_NAME, name, 256, &len))
    {
        QString desktopName = QString::fromLocal8Bit(name).toLower();
        if (desktopName == "default")
        {
            isLocked = false;
        }
    }
    
    CloseDesktop(hDesk);
    return isLocked;
}

void screenStatusCheck::onCheckStart()
{
    qDebug() << "[screenStatusCheck] ========================================";
    qDebug() << "[screenStatusCheck] 开始监控";
    qDebug() << "[screenStatusCheck] 当前桌面:" << getDesktopName();
    qDebug() << "[screenStatusCheck] 启动延迟:" << STARTUP_DELAY_MS << "ms";
    qDebug() << "[screenStatusCheck] ========================================";
    
    m_state = State::WaitingForStartup;
    
    m_startupDelayTimer = new QTimer(this);
    m_startupDelayTimer->setSingleShot(true);
    connect(m_startupDelayTimer, &QTimer::timeout, 
            this, &screenStatusCheck::onStartupDelayFinished);
    m_startupDelayTimer->start(STARTUP_DELAY_MS);
    
    m_checkTimer = new QTimer(this);
    connect(m_checkTimer, &QTimer::timeout, this, &screenStatusCheck::checkLockStatus);
    m_checkTimer->start(1000);
}

void screenStatusCheck::onStartupDelayFinished()
{
    bool currentlyLocked = isScreenLocked();
    
    qDebug() << "[screenStatusCheck] 启动延迟结束，当前锁屏:" << currentlyLocked;
    
    if (!currentlyLocked)
    {
        m_state = State::Unlocked;
        m_unlockTime = QDateTime::currentDateTime();
        qDebug() << "[screenStatusCheck] → Unlocked 状态";
    }
    else
    {
        m_state = State::Locked;
        qDebug() << "[screenStatusCheck] → Locked 状态";
    }
    
    m_stateConfirmCount = 0;
}

void screenStatusCheck::checkLockStatus()
{
    if (m_state == State::WaitingForStartup)
    {
        return;
    }
    
    bool currentlyLocked = isScreenLocked();
    
    switch (m_state)
    {
    case State::Locked:
        // 锁屏状态，检测解锁
        if (!currentlyLocked)
        {
            m_stateConfirmCount++;
            if (m_stateConfirmCount >= STATE_CONFIRM_COUNT)
            {
                qDebug() << "[screenStatusCheck] 检测到解锁";
                m_state = State::Unlocked;
                m_unlockTime = QDateTime::currentDateTime();
                m_stateConfirmCount = 0;
            }
        }
        else
        {
            m_stateConfirmCount = 0;
        }
        break;
        
    case State::Unlocked:
        // 解锁状态，检测锁屏
        if (currentlyLocked)
        {
            m_stateConfirmCount++;
            if (m_stateConfirmCount >= STATE_CONFIRM_COUNT)
            {
                qDebug() << "[screenStatusCheck] 检测到锁屏";
                m_state = State::Locked;
                m_stateConfirmCount = 0;
                performLogout();
            }
        }
        else
        {
            m_stateConfirmCount = 0;
        }
        break;
        
    default:
        break;
    }
}

void screenStatusCheck::performLogout()
{
    QDateTime now = QDateTime::currentDateTime();
    qint64 sessionSeconds = m_unlockTime.secsTo(now);
    
    qDebug() << "[screenStatusCheck] ========================================";
    qDebug() << "[screenStatusCheck] 会话时长:" << sessionSeconds << "秒";
    
    if (sessionSeconds < MIN_SESSION_SECONDS)
    {
        qDebug() << "[screenStatusCheck] 会话过短，忽略";
        qDebug() << "[screenStatusCheck] ========================================";
        return;
    }
    
    qDebug() << "[screenStatusCheck] 发送 logout 信号";
    qDebug() << "[screenStatusCheck] ========================================";
    
    emit logout();
    QCoreApplication::processEvents();
}

void screenStatusCheck::onSystemShutdown()
{
    qDebug() << "[screenStatusCheck] 收到关机通知，状态:" << static_cast<int>(m_state);
    
    if (m_checkTimer)
    {
        m_checkTimer->stop();
    }
    
    if (m_state == State::Unlocked)
    {
        QDateTime now = QDateTime::currentDateTime();
        qint64 sessionSeconds = m_unlockTime.secsTo(now);
        
        if (sessionSeconds >= MIN_SESSION_SECONDS)
        {
            qDebug() << "[screenStatusCheck] 关机登出";
            emit logout();
            QCoreApplication::processEvents();
            QThread::msleep(300);
        }
    }
}
