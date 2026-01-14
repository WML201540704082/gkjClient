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
    
    ///登录日志表相关操作
    bool updateLastLogoutTime();
    
private:
    QSqlDatabase db;
};

#endif // DATABASEMANAGER_H
