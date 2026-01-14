#ifndef SCREENSTATUSCHECK_H
#define SCREENSTATUSCHECK_H

#include <QObject>

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
    
private slots:
    void checkLockStatus();
    
};

#endif // SCREENSTATUSCHECK_H
