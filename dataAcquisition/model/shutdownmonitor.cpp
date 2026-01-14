#include "shutdownmonitor.h"
#include "utf8.h"

#include <QDebug>
#include <QCoreApplication>
#include <QThread>
#include <windows.h>

// ============= ShutdownWindow 实现 =============
ShutdownWindow::ShutdownWindow(QWidget *parent)
    : QWidget(parent)
    , m_hasNotified(false)
{
    // 创建隐藏窗口
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_DontShowOnScreen);
    setAttribute(Qt::WA_DeleteOnClose, false);
    
    // 显示窗口（虽然是隐藏的，但必须show才能接收消息）
    show();
    hide();
    
    qDebug() << "[ShutdownWindow] 隐藏窗口已创建，HWND:" << (void*)winId();
}

bool ShutdownWindow::nativeEvent(const QByteArray &eventType, 
                                 void *message, 
                                 long *result)
{
    if (eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG")
    {
        MSG* msg = static_cast<MSG*>(message);
        
        qDebug() << "[ShutdownWindow] 收到消息:" << hex << msg->message;
        
        // 监听关机消息
        if (msg->message == WM_QUERYENDSESSION)
        {
            qDebug() << "[ShutdownWindow] *** 收到 WM_QUERYENDSESSION ***";
            
            if (!m_hasNotified)
            {
                m_hasNotified = true;
                qDebug() << "[ShutdownWindow] 发送 systemShuttingDown 信号";
                
                emit systemShuttingDown();
                
                // 处理事件并等待
                QCoreApplication::processEvents();
                QThread::msleep(500); // 增加等待时间
                
                qDebug() << "[ShutdownWindow] 信号处理完成";
            }
            
            *result = TRUE;
            return true; // 改为true，阻止默认处理
        }
        
        if (msg->message == WM_ENDSESSION)
        {
            qDebug() << "[ShutdownWindow] *** 收到 WM_ENDSESSION，wParam=" << msg->wParam << " ***";
            
            if (msg->wParam && !m_hasNotified)
            {
                m_hasNotified = true;
                emit systemShuttingDown();
                QCoreApplication::processEvents();
                QThread::msleep(500);
            }
            return true;
        }
    }
    
    return QWidget::nativeEvent(eventType, message, result);
}

// ============= ShutdownMonitor 实现 =============
ShutdownMonitor::ShutdownMonitor(QObject *parent)
    : QObject(parent)
{
    qDebug() << "[ShutdownMonitor] 初始化，主线程ID:" << QThread::currentThreadId();
    
    // 创建隐藏窗口
    m_hiddenWindow = new ShutdownWindow();
    
    // 转发信号
    connect(m_hiddenWindow, &ShutdownWindow::systemShuttingDown,
            this, &ShutdownMonitor::systemShuttingDown);
    
    qDebug() << "[ShutdownMonitor] 关机监听器已启动";
}

ShutdownMonitor::~ShutdownMonitor()
{
    if (m_hiddenWindow)
    {
        m_hiddenWindow->close();
        delete m_hiddenWindow;
    }
}
