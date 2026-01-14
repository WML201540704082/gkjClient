#ifndef SCREENSTATUSCHECK_H
#define SCREENSTATUSCHECK_H

//#include <QObject>
//#include <QTimer>

//class screenStatusCheck : public QObject
//{
//    Q_OBJECT
//public:
//    explicit screenStatusCheck(QObject *parent = nullptr);
    
//    bool isScreenLocked();
    
//signals:
//    void logout();
    
//public slots:
//    void onCheckStart();
//    void onSystemShutdown(); // 新增：接收关机通知
    
//private slots:
//    void checkLockStatus();
    
//private:
//    void performLogout();
//};

#include <QObject>
#include <QTimer>
#include <QDateTime>

class screenStatusCheck : public QObject
{
    Q_OBJECT
public:
    explicit screenStatusCheck(QObject *parent = nullptr);
    
    bool isScreenLocked();
    
signals:
    void logout();
    
public slots:
    void onCheckStart();
    void onSystemShutdown();
    
private slots:
    void checkLockStatus();
    void onStartupDelayFinished();
    
private:
    void performLogout();
    QString getDesktopName();
    
    QTimer *m_checkTimer;
    QTimer *m_startupDelayTimer;
    
    enum class State {
        WaitingForStartup,    // 启动等待期
        Locked,               // 锁屏状态
        Unlocked              // 解锁状态
    };
    
    State m_state;
    int m_stateConfirmCount;
    QDateTime m_unlockTime;   // 解锁时间（用于计算会话时长）
    
    static const int STARTUP_DELAY_MS = 3000;
    static const int STATE_CONFIRM_COUNT = 2;
    static const int MIN_SESSION_SECONDS = 5;
};


#endif // SCREENSTATUSCHECK_H
