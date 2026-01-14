#ifndef SHUTDOWNMONITOR_H
#define SHUTDOWNMONITOR_H

#include <QObject>
#include <QWidget>
#include <QAbstractNativeEventFilter>

// 隐藏窗口类，专门接收系统消息
class ShutdownWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ShutdownWindow(QWidget *parent = nullptr);
    
signals:
    void systemShuttingDown();
    
protected:
    bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;
    
private:
    bool m_hasNotified;
};

// 监控器主类
class ShutdownMonitor : public QObject
{
    Q_OBJECT
public:
    explicit ShutdownMonitor(QObject *parent = nullptr);
    ~ShutdownMonitor();
    
signals:
    void systemShuttingDown();
    
private:
    ShutdownWindow *m_hiddenWindow;
};

#endif // SHUTDOWNMONITOR_H
