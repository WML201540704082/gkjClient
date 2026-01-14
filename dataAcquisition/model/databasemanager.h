#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QtSql/QSqlDatabase>

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
    
    ///
    /// \brief queryLatestPowerOnTime 查询最近一次开机时间
    /// \param powerOnTime
    /// \return 
    ///
    bool queryLatestPowerOnTime(QString &powerOnTime);
    
    ///
    /// \brief insertPowerOnTime 增加开机时间
    /// \param powderOnTime
    /// \return 
    ///
    bool insertPowerOnTime(QString powderOnTime);
    
    ///
    /// \brief updatePowerOffTime 更新关机时间
    /// \return 
    ///
    bool updatePowerOffTime();
    
    ///
    /// \brief queryUnsyncTime 查询未同步的开关机时间
    /// \param timeList
    /// \return 
    ///
    bool queryUnsyncTime(QList<QList<QString> > &timeList);
    
    ///
    /// \brief updateTimeSyncRecord 更新同步成功记录
    /// \param powerOnTime
    /// \param powerOffTime
    /// \return 
    ///
    bool updateTimeSyncRecord(QString powerOnTime, QString powerOffTime);
    
private:
    QSqlDatabase db;
};

#endif // DATABASEMANAGER_H
