#ifndef DATABASEMANAGER2_H
#define DATABASEMANAGER2_H

#include <QtSql/QSqlDatabase>

class databaseManager2
{
public:
    databaseManager2();
    ~databaseManager2();

    ///
    /// \brief connectToDatabase 连接数据库
    /// \return
    ///
    bool connectToDatabase(); 
    
    ///
    /// \brief queryLatestLoginRecord 查找最近一次登录的用户名和部门
    /// \param username
    /// \param department
    /// \return 
    ///
    bool queryLatestLoginRecord(QString& username, QString& department);
    
private:
    QSqlDatabase db;
    
};

#endif // DATABASEMANAGER2_H
