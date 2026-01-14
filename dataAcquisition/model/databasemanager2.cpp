#include "databasemanager2.h"

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QString>
#include <QDebug>
#include <QCoreApplication>
#include <QThread>

databaseManager2::databaseManager2()
{
    
}

databaseManager2::~databaseManager2()
{
    if (db.isOpen())
    {
        QString strName=db.connectionName();
        db.close();
        db=QSqlDatabase();
        QSqlDatabase::removeDatabase(strName);
    }
}

bool databaseManager2::connectToDatabase()
{
    QString threadConnectionName = QString("connection_%1").arg(reinterpret_cast<quintptr>(QThread::currentThreadId()));
    qDebug()<<"threadConnectionName: "<<threadConnectionName<<", currentThreadId: "<<QThread::currentThreadId();
    if (QSqlDatabase::contains(threadConnectionName))
    {
        db = QSqlDatabase::database(threadConnectionName);
    }
    else
    {
        db = QSqlDatabase::addDatabase("QSQLITE", threadConnectionName);
        db.setDatabaseName(QCoreApplication::applicationDirPath() + "/face_recognition.db");
        
        if (db.open())
        {
            QSqlQuery query(db);
            // 1. 设置查询只读模式（可选但推荐）
            query.exec("PRAGMA query_only = ON;");
            
            // 2. WAL模式已由进程A设置，这里再设置一次也无妨（幂等操作）
            query.exec("PRAGMA journal_mode=WAL;");
            
            // 3. 设置busy超时（重要！防止读取时遇到锁）
            query.exec("PRAGMA busy_timeout = 5000;");  // 5秒超时
        }
        else
        {
            qWarning() << "Failed to open database for thread:" << threadConnectionName << " Error:" << db.lastError().text();
            return false;
        }
    }
    return true;
}

bool databaseManager2::queryLatestLoginRecord(QString &username, QString &department)
{
    QSqlQuery query("SELECT username, department FROM LoginLog "
                    "WHERE login_time = (SELECT MAX(login_time) FROM LoginLog)"
                    "AND (logout_time IS NULL OR logout_time = '');", db);
    if (!query.exec())
    {
        qDebug() << "Error: Unable to update logout time!" << query.lastError().text();
        return false;
    }
    else if(!query.next())
    {
        qDebug() << "LoginLog data is empty";
        return true;
    }
    else
    {
        username = query.value(0).toString();
        department = query.value(1).toString();
        return true;
    }

    return false;
}
