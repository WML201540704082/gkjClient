#include "databasemanager.h"
#include "utf8.h"

#include <QDebug>
#include <QCoreApplication>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QSqlDriver>
#include <QThread>
#include <QDateTime>

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
        db.setDatabaseName(QCoreApplication::applicationDirPath() + "/face_recognition.db");
        
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

    // 创建管理员表
    if (!query.exec("CREATE TABLE IF NOT EXISTS Admin ("
                    "id TEXT PRIMARY KEY, "
                    "username TEXT NOT NULL UNIQUE, "
                    "user_password TEXT NOT NULL,"
                    "face_image_path TEXT NOT NULL,"
                    "department TEXT NOT NULL);"))
    {
        qDebug() << "Error: Unable to create Admin table!" << query.lastError().text();
        return false;
    }

    // 创建临时授权用户表
    if (!query.exec("CREATE TABLE IF NOT EXISTS TemporaryUser ("
                    "id TEXT PRIMARY KEY, "
                    "username TEXT NOT NULL UNIQUE, "
                    "user_password TEXT NOT NULL,"
                    "face_image_path TEXT NOT NULL,"
                    "department TEXT NOT NULL);"))
    {
        qDebug() << "Error: Unable to create TemporaryUser table!" << query.lastError().text();
        return false;
    }

    // 创建使用时间段表
    if (!query.exec("CREATE TABLE IF NOT EXISTS UsagePeriods  ("
                    "id TEXT PRIMARY KEY, "
                    "temp_user_id TEXT NOT NULL, "
                    "start_time TEXT NOT NULL, "
                    "end_time TEXT NOT NULL, "
                    "FOREIGN KEY (temp_user_id) REFERENCES TemporaryUser(id) ON DELETE CASCADE);"))
    {
        qDebug() << "Error: Unable to create UsagePeriods table!" << query.lastError().text();
        return false;
    }
    
    //创建系统用户名密码表
    if (!query.exec("CREATE TABLE IF NOT EXISTS  Sys ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "username TEXT NOT NULL UNIQUE, "
                    "password TEXT NOT NULL);"))
    {
        qDebug() << "Error: Unable to create Admin table!" << query.lastError().text();
        return false;
    }
    
    // 创建配置表
    if (!query.exec("CREATE TABLE IF NOT EXISTS Configuration ("
                    "id INTEGER PRIMARY KEY CHECK (id = 1), "
                    "TempUserLogin INTEGER NOT NULL DEFAULT 0, "
                    "LoginIdentity TEXT,"
                    "SaveFilePath TEXT NOT NULL);"))
    {
        qDebug() << "Error: Unable to create Configuration table!" << query.lastError().text();
        return false;
    }
    
    // 创建登录日志表
    if (!query.exec("CREATE TABLE IF NOT EXISTS LoginLog ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "username TEXT NOT NULL, "
                    "login_time TEXT NOT NULL,"
                    "logout_time TEXT,"
                    "department TEXT NOT NULL,"
                    "synced INTEGER NOT NULL DEFAULT 0);"))
    {
        qDebug() << "Error: Unable to create LoginLog table!" << query.lastError().text();
        return false;
    }
    
    // 为常用查询字段创建索引
    if (!query.exec("CREATE INDEX IF NOT EXISTS idx_login_time ON LoginLog(login_time DESC);"))
    {
        qDebug() << "Error: Unable to create index on login_time!" << query.lastError().text();
        return false;
    }
    
    // 创建同步日志表
    if (!query.exec("CREATE TABLE IF NOT EXISTS SyncLog ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "table_name TEXT NOT NULL, "
                    "record_id TEXT NOT NULL, "
                    "operation_type TEXT NOT NULL, "
                    "operation_time TEXT NOT NULL, "
                    "synced INTEGER NOT NULL DEFAULT 0);"))
    {
        qDebug() << "Error: Unable to create SyncLog table!" << query.lastError().text();
        return false;
    }
    
    // 创建同步配置表
    if (!query.exec("CREATE TABLE IF NOT EXISTS SyncConfig ("
                    "id INTEGER PRIMARY KEY CHECK (id = 1), "
                    "last_sync_time TEXT, "
                    "last_sync_to_server TEXT, "
                    "last_sync_from_server TEXT);"))
    {
        qDebug() << "Error: Unable to create SyncConfig table!" << query.lastError().text();
        return false;
    }
    
    // 同步日志表索引
    if (!query.exec("CREATE INDEX IF NOT EXISTS idx_sync_log_time ON SyncLog(operation_time);"))
    {
        qDebug() << "Error: Unable to create index on SyncLog operation_time!" << query.lastError().text();
        return false;
    }
    
    if (!query.exec("CREATE INDEX IF NOT EXISTS idx_sync_log_synced ON SyncLog(synced);"))
    {
        qDebug() << "Error: Unable to create index on SyncLog synced!" << query.lastError().text();
        return false;
    }
    
    if (!query.exec("CREATE INDEX IF NOT EXISTS idx_sync_log_table_record ON SyncLog(table_name, record_id);"))
    {
        qDebug() << "Error: Unable to create index on SyncLog table_name and record_id!" << query.lastError().text();
        return false;
    }
    
    // ========== 创建触发器 ==========
    
    // Admin表触发器
    if (!query.exec("CREATE TRIGGER IF NOT EXISTS trg_admin_insert "
                    "AFTER INSERT ON Admin "
                    "FOR EACH ROW "
                    "BEGIN "
                    "  INSERT INTO SyncLog(table_name, record_id, operation_type, operation_time) "
                    "  VALUES('Admin', NEW.id, 'add', datetime('now', 'localtime')); "
                    "END;"))
    {
        qDebug() << "Error: Unable to create trigger trg_admin_insert!" << query.lastError().text();
        return false;
    }
    
    if (!query.exec("CREATE TRIGGER IF NOT EXISTS trg_admin_update "
                    "AFTER UPDATE ON Admin "
                    "FOR EACH ROW "
                    "BEGIN "
                    "  INSERT INTO SyncLog(table_name, record_id, operation_type, operation_time) "
                    "  VALUES('Admin', NEW.id, 'update', datetime('now', 'localtime')); "
                    "END;"))
    {
        qDebug() << "Error: Unable to create trigger trg_admin_update!" << query.lastError().text();
        return false;
    }
    
    if (!query.exec("CREATE TRIGGER IF NOT EXISTS trg_admin_delete "
                    "AFTER DELETE ON Admin "
                    "FOR EACH ROW "
                    "BEGIN "
                    "  INSERT INTO SyncLog(table_name, record_id, operation_type, operation_time) "
                    "  VALUES('Admin', OLD.id, 'delete', datetime('now', 'localtime')); "
                    "END;"))
    {
        qDebug() << "Error: Unable to create trigger trg_admin_delete!" << query.lastError().text();
        return false;
    }
    
    // TemporaryUser表触发器
    if (!query.exec("CREATE TRIGGER IF NOT EXISTS trg_temp_user_insert "
                    "AFTER INSERT ON TemporaryUser "
                    "FOR EACH ROW "
                    "BEGIN "
                    "  INSERT INTO SyncLog(table_name, record_id, operation_type, operation_time) "
                    "  VALUES('TemporaryUser', NEW.id, 'add', datetime('now', 'localtime')); "
                    "END;"))
    {
        qDebug() << "Error: Unable to create trigger trg_temp_user_insert!" << query.lastError().text();
        return false;
    }
    
    if (!query.exec("CREATE TRIGGER IF NOT EXISTS trg_temp_user_update "
                    "AFTER UPDATE ON TemporaryUser "
                    "FOR EACH ROW "
                    "BEGIN "
                    "  INSERT INTO SyncLog(table_name, record_id, operation_type, operation_time) "
                    "  VALUES('TemporaryUser', NEW.id, 'update', datetime('now', 'localtime')); "
                    "END;"))
    {
        qDebug() << "Error: Unable to create trigger trg_temp_user_update!" << query.lastError().text();
        return false;
    }
    
    if (!query.exec("CREATE TRIGGER IF NOT EXISTS trg_temp_user_delete "
                    "AFTER DELETE ON TemporaryUser "
                    "FOR EACH ROW "
                    "BEGIN "
                    "  INSERT INTO SyncLog(table_name, record_id, operation_type, operation_time) "
                    "  VALUES('TemporaryUser', OLD.id, 'delete', datetime('now', 'localtime')); "
                    "END;"))
    {
        qDebug() << "Error: Unable to create trigger trg_temp_user_delete!" << query.lastError().text();
        return false;
    }
    
    // UsagePeriods表触发器
    if (!query.exec("CREATE TRIGGER IF NOT EXISTS trg_usage_periods_insert "
                    "AFTER INSERT ON UsagePeriods "
                    "FOR EACH ROW "
                    "BEGIN "
                    "  INSERT INTO SyncLog(table_name, record_id, operation_type, operation_time) "
                    "  VALUES('UsagePeriods', CAST(NEW.id AS TEXT), 'add', datetime('now', 'localtime')); "
                    "END;"))
    {
        qDebug() << "Error: Unable to create trigger trg_usage_periods_insert!" << query.lastError().text();
        return false;
    }
    
    if (!query.exec("CREATE TRIGGER IF NOT EXISTS trg_usage_periods_update "
                    "AFTER UPDATE ON UsagePeriods "
                    "FOR EACH ROW "
                    "BEGIN "
                    "  INSERT INTO SyncLog(table_name, record_id, operation_type, operation_time) "
                    "  VALUES('UsagePeriods', CAST(NEW.id AS TEXT), 'update', datetime('now', 'localtime')); "
                    "END;"))
    {
        qDebug() << "Error: Unable to create trigger trg_usage_periods_update!" << query.lastError().text();
        return false;
    }
    
    if (!query.exec("CREATE TRIGGER IF NOT EXISTS trg_usage_periods_delete "
                    "AFTER DELETE ON UsagePeriods "
                    "FOR EACH ROW "
                    "BEGIN "
                    "  INSERT INTO SyncLog(table_name, record_id, operation_type, operation_time) "
                    "  VALUES('UsagePeriods', CAST(OLD.id AS TEXT), 'delete', datetime('now', 'localtime')); "
                    "END;"))
    {
        qDebug() << "Error: Unable to create trigger trg_usage_periods_delete!" << query.lastError().text();
        return false;
    }
    
    return true;
}

//bool databaseManager::updateLastLogoutTime()
//{
//    QSqlQuery query(db);
    
//    query.prepare("UPDATE LoginLog SET logout_time = :logout_time "
//                  "WHERE login_time = (SELECT MAX(login_time) FROM LoginLog)"
//                  "AND (logout_time IS NULL OR logout_time = '');");
//    query.bindValue(":logout_time", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    
//    if (!query.exec())
//    {
//        qDebug() << "Error: Unable to update logout time!" << query.lastError().text();
//        return false;
//    }
    
////    return query.numRowsAffected() > 0;
//    return true;
//}

bool databaseManager::updateLastLogoutTime()
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
    
    query.prepare("UPDATE LoginLog SET logout_time = :logout_time "
                  "WHERE login_time = (SELECT MAX(login_time) FROM LoginLog) "
                  "AND (logout_time IS NULL OR logout_time = '');");
    query.bindValue(":logout_time", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    
    if (!query.exec())
    {
        qDebug() << "[DB] ××× 更新失败:" << query.lastError().text();
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
    checkQuery.exec("SELECT logout_time FROM LoginLog ORDER BY login_time DESC LIMIT 1");
    if (checkQuery.next())
    {
        QString logoutTime = checkQuery.value(0).toString();
        qDebug() << "[DB] 验证：最新的logout_time =" << logoutTime;
    }
    
    qDebug() << "[DB] ====== 数据库更新完成 ======";
    return true;
}

