#include "databasemanager.h"

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QString>
#include <QDebug>
#include <QCoreApplication>
#include <QThread>
#include <QDateTime>
#include <QSqlDriver>

databaseManager::databaseManager()
{
    
}

databaseManager::~databaseManager()
{
    if (db.isOpen())
    {
        QString strName=db.connectionName();
        db.close();
        db=QSqlDatabase();
        QSqlDatabase::removeDatabase(strName);
    }
}

bool databaseManager::connectToDatabase()
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
        db.setDatabaseName(QCoreApplication::applicationDirPath() + "/data_acquisition.db");
        
        if (db.open())
        {
            QSqlQuery query(db);
            query.exec("PRAGMA foreign_keys = ON;");
        }
        else
        {
            qWarning() << "Failed to open database for thread:" << threadConnectionName << " Error:" << db.lastError().text();
            return false;
        }
    }
    return true;
}


bool databaseManager::createTables()
{
    QSqlQuery query(db);
    
    // 创建登录日志表
    if (!query.exec("CREATE TABLE IF NOT EXISTS powerOnOff ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "power_on_time TEXT NOT NULL,"
                    "power_off_time TEXT,"
                    "synced INTEGER NOT NULL DEFAULT 0);"))
    {
        qDebug() << "Error: Unable to create LoginLog table!" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool databaseManager::queryLatestPowerOnTime(QString &powerOnTime)
{
    QSqlQuery query("SELECT power_on_time FROM powerOnOff ORDER BY power_on_time DESC LIMIT 1", db);
    if (!query.exec())
    {
        qDebug() << "Error querying power_on_time:" << query.lastError().text();
        return false;
    }
    else if(!query.next())
    {
        qDebug() << "powerOnOff data is empty";
        return true;
    }
    else
    {
        powerOnTime = query.value(0).toString();
        return true;
    }
    return false;
}

bool databaseManager::insertPowerOnTime(QString powderOnTime)
{
    QSqlQuery query(db);
    query.prepare("INSERT INTO powerOnOff (power_on_time) "
                  "VALUES (?)");
    query.addBindValue(powderOnTime);
    if (!query.exec())
    {
        qDebug() << "Error: Failed to record power_on_time!" << query.lastError().text();
        return false;
    }
    return true;
}

bool databaseManager::updatePowerOffTime()
{
    qDebug() << "[DB] ====== 开始更新数据库 ======";
    qDebug() << "[DB] 线程ID:" << QThread::currentThreadId();
    qDebug() << "[DB] 时间:" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    
    // 确保之前的事务已提交
    if (db.driver()->hasFeature(QSqlDriver::Transactions))
    {
        qDebug() << "[DB] 提交之前的事务";
        db.commit();
    }
    
    QSqlQuery query(db);
    query.prepare("UPDATE powerOnOff SET power_off_time = :power_off_time "
                  "WHERE power_on_time = (SELECT MAX(power_on_time) FROM powerOnOff) "
                  "AND (power_off_time IS NULL OR power_off_time = '');");
    query.bindValue(":power_off_time", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        
    if (!query.exec())
    {
        qDebug() << "Error: Unable to update power_off_time!" << query.lastError().text();
        return false;
    }
    
    qDebug() << "[DB] SQL执行成功，影响行数:" << query.numRowsAffected();
    
    // 立即提交事务
    if (db.driver()->hasFeature(QSqlDriver::Transactions))
    {
        qDebug() << "[DB] 提交事务";
        db.commit();
    }
    
    // SQLite特有：强制同步到磁盘
    QSqlQuery syncQuery(db);
    syncQuery.exec("PRAGMA synchronous = FULL;");
    syncQuery.exec("PRAGMA journal_mode = DELETE;"); // 确保立即写入
    
    // 验证更新
    QSqlQuery checkQuery(db);
    checkQuery.exec("SELECT power_off_time FROM powerOnOff ORDER BY power_on_time DESC LIMIT 1");
    if (checkQuery.next())
    {
        QString powerOffTime = checkQuery.value(0).toString();
        qDebug() << "[DB] power_off_time =" << powerOffTime;
    }
    
    qDebug() << "[DB] ====== 数据库更新完成 ======";
    
//    return query.numRowsAffected() > 0;
    return true;
}

bool databaseManager::queryUnsyncTime(QList<QList<QString> > &timeList)
{
    QSqlQuery query("SELECT power_on_time, power_off_time FROM powerOnOff WHERE synced = 0", db);
    if (!query.exec())
    {
        qWarning() << "Error querying powerOnOff data:" << query.lastError().text();
        return false;
    }

    while (query.next())
    {
        QList<QString> time;
        time.append(query.value(0).toString());
        time.append(query.value(1).toString());
        
        timeList.append(time);
    }
    return true;
}

bool databaseManager::updateTimeSyncRecord(QString powerOnTime, QString powerOffTime)
{
    QSqlQuery query(db);
    
    query.prepare("UPDATE powerOnOff SET synced = 1 WHERE power_on_time = ? AND power_off_time = ?");
    query.addBindValue(powerOnTime);
    query.addBindValue(powerOffTime);
    
    if (!query.exec())
    {
        qWarning() << "Error updating LoginLog data:" << query.lastError().text();
        return false;
    }
    return true;
}
