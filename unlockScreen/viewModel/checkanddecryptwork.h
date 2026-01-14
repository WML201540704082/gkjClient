#ifndef CHECKANDDECRYPTWORK_H
#define CHECKANDDECRYPTWORK_H

#include "model/databasemanager.h"
#include "model/mysm4.h"

#include <QObject>
#include <QMap>
#include <tuple>

class checkAndDecryptWork : public QObject
{
    Q_OBJECT
public:
    explicit checkAndDecryptWork(QObject *parent = nullptr);
 
public slots:
    void onStartCheckAndDecrypt();
    
signals:    
    void sendAdminData(unsigned char *data, size_t dataLen, QString username, QString pwd, QString department);
    
    void sendTmpUserData(unsigned char *adminData, size_t adminDataLen,
                         QString adminUsername, QString adminPwd, QString department,
                         QList<std::tuple<QString, unsigned char *, size_t, QString, QString, QString>> decryptedTmpUserList);
    
    void threadErrorFinished(QString str);
    
    
private:  
    bool tmpUserLogin = false;
    mySm4 *pSm4 = nullptr;
};
#endif // CHECKANDDECRYPTWORK_H
