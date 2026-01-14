#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QtSql/QSqlDatabase>
#include <QDateTime>

class databaseManager
{
public:
    databaseManager();
    ~databaseManager();

    ///
    /// \brief connectToDatabase 连接数据库
    /// \return
    ///
    bool connectToDatabase(); 
    ///
    /// \brief createTables 创建表
    /// \return
    ///
    bool createTables();

    /// 管理员相关操作
    bool insertAdminData(const QString &uuid, const QString &username, const QString &userPwd, const QString &faceImagePath, const QString &department);
    bool queryAdminData(QString &uuid, QString &username, QString &userPwd, QString &faceImagePath, QString &department);
    bool queryAdminPwd(QString &userPwd);
    bool deleteAdminData(const QString &username);
    bool deleteAdminDataById(const QString &uuid);
    bool deleteAdminData();

    /// 临时授权用户相关操作
    bool insertTempUserData(const QString &uuid, const QString &username, const QString &userPwd, const QString &faceImagePath, const QString &department);
    bool updateTempUserData(const QString &uuid, const QString &username, const QString &userPwd, const QString &faceImagePath, const QString &department);
    bool deleteTempUserData(const QString &username);
    bool deleteTempUserDataById(const QString &uuid);
    bool queryTempUserData(QList<QList<QString>> &tempUsers);
    bool queryTempUserDataByName(const QString &username, QList<QList<QString>> &tempUsers);
    bool queryTempUserDataById(const QString &uuid, QList<QString> &tempUser);
    bool queryTempUserIdByName(const QString &username, QString &tempUserId);
    bool queryTempUserImgPathById(const QString &uuid, QString &imgPath);
    
    /// 临时授权用户使用时间段相关操作
    bool insertUsagePeriodData(const QString &timeId, const QString &tempUserId, const QString& startTime, const QString& endTime);
    bool updateUsagePeriodData(const QString &tempUserId, const QString& oldStartTime, const QString& oldEndTime, const QString& startTime, const QString& endTime);
    bool deleteUsagePeriodData(const QString &tempUserId, const QString& startTime, const QString& endTime);
    bool queryUsagePeriodsData(const QString &tempUserId, QList<QPair<QString, QString>>& periods);
    bool queryUsagePeriodsData(const QString &timeId, QList<QString> &list);
    
    /// 获取临时用户数据
    QList<QPair<QString, QPair<QString, QString>>> getAllTemporaryUsersWithUsagePeriods();
    QList<QPair<QString, QString>> getUsagePeriodsForTemporaryUser(const QString &userId);

    ///系统用户名密码表相关操作
    bool insertSysData(const QString &username, const QString& pwd);
    bool querySysData(QString &username, QString& pwd);
    bool deleteSysData();
    bool querySysPwd(QString& pwd);
    
    ///配置表相关操作
    bool insertConfigData(bool tmpUserLogin, QString LoginIdentity);
    QString querySaveFilePath();
    bool queryTempUserLogin();
    QString queryLoginIdentity();
    bool updateTempUserLogin(bool tmpUserLogin);
    bool updateLoginIdentity(QString LoginIdentity);
    
    ///登录日志表相关操作
    bool updateLastLogoutTime();
    bool quertUnsyncLog(QList<QList<QString>> &logList);
    bool updateLogSyncRecord(QString username, QString loginTime, QString logoutTime);
    
    ///登录日志表相关操作
    bool cleanOldLoginLogs(int daysToKeep);
    
    ///同步记录表相关操作
    bool getUnsyncedRecord(QList<QList<QString>> &recordsList);
    bool getSimplifiedRecord(QList<QList<QString>> &recordsList);
    bool updateRecord(QString recordId);
    
private:
    QSqlDatabase db;
};

#endif // DATABASEMANAGER_H
