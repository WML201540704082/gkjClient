#include "databasemanager.h"

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QString>
#include <QDebug>
#include <QCoreApplication>
#include <QThread>

databaseManager::databaseManager()
{
//    connectToDatabase();
//    createTables();
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
//    if (QSqlDatabase::contains("qt_sql_default_connection"))
//    {
//        db = QSqlDatabase::database("qt_sql_default_connection");
//    }
//    else
//    {
//        db = QSqlDatabase::addDatabase("QSQLITE");
//        db.setDatabaseName(QCoreApplication::applicationDirPath() + "/face_recognition.db");
//    }


//    if (db.open())
//    {
//        QSqlQuery query(db);
//        query.exec("PRAGMA foreign_keys = ON;");
//    }
//    else
//    {
//        qWarning() << "Failed to open database:" << db.lastError().text();
//        return false;
//    }
//    return true;
    
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
            query.exec("PRAGMA journal_mode=WAL;");
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

bool databaseManager::insertAdminData(const QString &uuid, const QString &username, const QString &userPwd, const QString &faceImagePath, const QString &department)
{
    QSqlQuery query(db);

    // 检查管理员是否已经存在
    query.prepare("SELECT COUNT(*) FROM Admin");
    if (!query.exec() || !query.next())
    {
        qWarning() << "Error checking Admin table:" << query.lastError().text();
        return false;
    }

    if (query.value(0).toInt() > 0)
    {
        qWarning() << "Admin already exists!";
        return false;
    }

    // 插入管理员数据
    query.prepare("INSERT INTO Admin (id, username, user_password, face_image_path, department) VALUES (?, ?, ?, ?,?)");
    query.addBindValue(uuid);
    query.addBindValue(username);
    query.addBindValue(userPwd);
    query.addBindValue(faceImagePath);
    query.addBindValue(department);

    if (!query.exec())
    {
        qWarning() << "Error inserting admin data:" << query.lastError().text();
        return false;
    }
    return true;
}

bool databaseManager::queryAdminData(QString &uuid, QString &username, QString &userPwd, QString &faceImagePath, QString &department)
{
    QSqlQuery query("SELECT * FROM Admin", db);
    if (!query.exec() /*|| !query.next()*/)
    {
        qDebug() << "Error querying admin data:" << query.lastError().text();
        return false;
    }
    else if(!query.next())
    {
        qDebug() << "admin data is empty";
        return true;
    }
    else
//    if (query.next())
    {
        uuid = query.value(0).toString();
        username = query.value(1).toString();
        userPwd = query.value(2).toString();
        faceImagePath = query.value(3).toString();
        department = query.value(4).toString();
        return true;
    }

    return false;
}

bool databaseManager::queryAdminPwd(QString &userPwd)
{
    QSqlQuery query("SELECT user_password FROM Admin", db);
    if (!query.exec())
    {
        qDebug() << "Error querying admin data:" << query.lastError().text();
        return false;
    }
    else if(!query.next())
    {
        qDebug() << "admin data is empty";
        return true;
    }
    else
    {
        userPwd = query.value(0).toString();
        return true;
    }
    return false;
}

bool databaseManager::deleteAdminData(const QString &username)
{
    QSqlQuery query(db);
    query.prepare("DELETE FROM Admin WHERE username = ?");

    query.addBindValue(username);

    if (!query.exec())
    {
        qWarning() << "Error deleting Admin data:" << query.lastError().text();
        return false;
    }
    return true;
}

bool databaseManager::deleteAdminDataById(const QString &uuid)
{
    QSqlQuery query(db);
    query.prepare("DELETE FROM Admin WHERE id = ?");

    query.addBindValue(uuid);

    if (!query.exec())
    {
        qWarning() << "Error deleting Admin data by id:" << query.lastError().text();
        return false;
    }
    return true;
}

bool databaseManager::deleteAdminData()
{
    QSqlQuery query(db);
    query.prepare("DELETE FROM Admin");
    
    if (!query.exec())
    {
        qWarning() << "Error deleting Admin data:" << query.lastError().text();
        return false;
    }
    return true;
}

bool databaseManager::insertTempUserData(const QString &uuid, const QString &username, const QString &userPwd, const QString &faceImagePath, const QString &department)
{
    QSqlQuery query(db);
    query.prepare("INSERT INTO TemporaryUser (id, username, user_password, face_image_path, department) VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(uuid);
    query.addBindValue(username);
    query.addBindValue(userPwd);
    query.addBindValue(faceImagePath);
    query.addBindValue(department);

    if (!query.exec())
    {
        qWarning() << "Error inserting temporary user data:" << query.lastError().text();
        return false;
    }
    return true;
}

bool databaseManager::updateTempUserData(const QString &uuid, const QString &username, const QString &userPwd, const QString &faceImagePath, const QString &department)
{
    QSqlQuery query(db);
    query.prepare("UPDATE TemporaryUser SET username = ?, user_password = ?, face_image_path = ?, department = ? WHERE id = ?");
    query.addBindValue(username);
    query.addBindValue(userPwd);
    query.addBindValue(faceImagePath);
    query.addBindValue(uuid);
    query.addBindValue(department);

    if (!query.exec())
    {
        qWarning() << "Error updating temporary user data:" << query.lastError().text();
        return false;
    }
    return true;
}

bool databaseManager::deleteTempUserData(const QString &username)
{
    QSqlQuery query(db);
    query.prepare("DELETE FROM TemporaryUser WHERE username = ?");
    query.addBindValue(username);

    if (!query.exec())
    {
        qWarning() << "Error deleting temporary user data:" << query.lastError().text();
        return false;
    }
    return true;
}

bool databaseManager::deleteTempUserDataById(const QString &uuid)
{
    QSqlQuery query(db);
    query.prepare("DELETE FROM TemporaryUser WHERE id = ?");
    query.addBindValue(uuid);
    
    if (!query.exec())
    {
        qWarning() << "Error deleting temporary user data by id:" << query.lastError().text();
        return false;
    }
    return true;
}

bool databaseManager::queryTempUserData(QList<QList<QString> > &tempUsers)
{
    QSqlQuery query("SELECT * FROM TemporaryUser", db);

    if (!query.exec())
    {
        qWarning() << "Error querying temporary user data:" << query.lastError().text();
        return false;
    }

    while (query.next())
    {
        QList<QString> tempUser;
        tempUser.append(query.value(0).toString());
        tempUser.append(query.value(1).toString());
        tempUser.append(query.value(2).toString());
        tempUser.append(query.value(3).toString());
        
        tempUsers.append(tempUser);
    }

    return !tempUsers.isEmpty();
}



bool databaseManager::queryTempUserDataByName(const QString &username, QList<QList<QString> > &tempUsers)
{
    QString likeStr = "%" + username + "%";
    QSqlQuery query("SELECT * FROM TemporaryUser WHERE username LIKE ?", db);
    query.addBindValue(likeStr);

    if (!query.exec())
    {
        qWarning() << "Error querying temporary user data:" << query.lastError().text();
        return false;
    }

    while (query.next())
    {
//        QMap<QString, QString> tempUser;
        QString tmpUuid = query.value(0).toString();
        QString tmpUsername = query.value(1).toString();
        QString tmpUserPwd = query.value(2).toString();
        QString tmpImgPath = query.value(3).toString();
        
        QList<QString> tmpList;
        tmpList << tmpUuid<< tmpUsername << tmpUserPwd  << tmpImgPath;
        tempUsers.append(tmpList);
    }
    return !tempUsers.isEmpty();
}

bool databaseManager::queryTempUserDataById(const QString &uuid, QList<QString> &tmpList)
{
    QSqlQuery query(db);
    query.prepare("SELECT * FROM TemporaryUser WHERE id = ?");
    query.addBindValue(uuid);
    
    if (!query.exec())
    {
        qWarning() << "Error querying TemporaryUser data:" << query.lastError().text();
        return false;
    }

    while (query.next())
    {
        QString tmpUsername = query.value(1).toString();
        QString tmpUserPwd = query.value(2).toString();
        QString tmpImgPath = query.value(3).toString();
        QString tmpUserDepartment = query.value(4).toString();
        
        tmpList << tmpUsername << tmpUserPwd  << tmpImgPath << tmpUserDepartment;
    }

    return true;
}

bool databaseManager::queryTempUserIdByName(const QString &username, QString &tempUserId)
{
    QSqlQuery query("SELECT id FROM TemporaryUser WHERE username = ?", db);
    query.addBindValue(username);

    if (!query.exec())
    {
        qWarning() << "Error querying temporary user data:" << query.lastError().text();
        return false;
    }

    if(query.next())
    {
        tempUserId = query.value(0).toString();
        return true;
    }

    return false;
}

bool databaseManager::queryTempUserImgPathById(const QString &uuid, QString &imgPath)
{
    QSqlQuery query("SELECT face_image_path FROM TemporaryUser WHERE id = ?", db);
    query.addBindValue(uuid);
    
    if (!query.exec())
    {
        qWarning() << "Error querying temporary user image path:" << query.lastError().text();
        return false;
    }
    
    if(query.next())
    {
        imgPath = query.value(0).toString();
        return true;
    }
    
    return false;
}

bool databaseManager::insertUsagePeriodData(const QString &timeId, const QString &tempUserId, const QString &startTime, const QString &endTime)
{
    QSqlQuery query(db);
    query.prepare("INSERT INTO UsagePeriods (id, temp_user_id, start_time, end_time) VALUES (?, ?, ?, ?)");
    query.addBindValue(timeId);
    query.addBindValue(tempUserId);
    query.addBindValue(startTime);
    query.addBindValue(endTime);

    if (!query.exec())
    {
        qWarning() << "Error inserting UsagePeriods data:" << query.lastError().text();
        return false;
    }
    return true;
}

bool databaseManager::updateUsagePeriodData(const QString &tempUserId, const QString &oldStartTime, const QString &oldEndTime, const QString &startTime, const QString &endTime)
{
    QSqlQuery query(db);
    query.prepare("UPDATE UsagePeriods SET start_time = ?, end_time = ? WHERE temp_user_id = ? AND start_time = ? AND end_time = ?");
    query.addBindValue(startTime);
    query.addBindValue(endTime);
    query.addBindValue(tempUserId);
    query.addBindValue(oldStartTime);
    query.addBindValue(oldEndTime);

    if (!query.exec())
    {
        qWarning() << "Error updating UsagePeriods data:" << query.lastError().text();
        return false;
    }
    return true;
}

bool databaseManager::deleteUsagePeriodData(const QString &tempUserId, const QString &startTime, const QString &endTime)
{
    QSqlQuery query(db);
    query.prepare("DELETE FROM UsagePeriods WHERE temp_user_id = ? AND start_time = ? AND end_time = ?");
    query.addBindValue(tempUserId);
    query.addBindValue(startTime);
    query.addBindValue(endTime);

    if (!query.exec())
    {
        qWarning() << "Error deleting UsagePeriods data:" << query.lastError().text();
        return false;
    }
    return true;
}

bool databaseManager::queryUsagePeriodsData(const QString &tempUserId, QList<QPair<QString, QString> > &periods)
{
    QSqlQuery query(db);
    query.prepare("SELECT start_time, end_time FROM UsagePeriods WHERE temp_user_id = ?");
    query.addBindValue(tempUserId);

    if (!query.exec())
    {
        qWarning() << "Error querying UsagePeriods data:" << query.lastError().text();
        return false;
    }

    while (query.next())
    {
        QString start = query.value(0).toString();
        QString end = query.value(1).toString();
        periods.append(qMakePair(start, end));
    }

    return true;
}

bool databaseManager::queryUsagePeriodsData(const QString &timeId, QList<QString> &list)
{
    QSqlQuery query(db);
    query.prepare("SELECT temp_user_id, start_time, end_time FROM UsagePeriods WHERE id = ?");
    query.addBindValue(timeId);
    
    if (!query.exec())
    {
        qWarning() << "Error querying UsagePeriods data:" << query.lastError().text();
        return false;
    }
    
    while (query.next())
    {
        QString tmpUserId = query.value(0).toString();
        QString start = query.value(1).toString();
        QString end = query.value(2).toString();
        list << tmpUserId << start << end;
    }

    return true;
}

QList<QPair<QString, QPair<QString, QString> > > databaseManager::getAllTemporaryUsersWithUsagePeriods()
{
    QList<QPair<QString, QPair<QString, QString>>> result;  // 用于存储临时用户和其使用时间段的开始和结束时间
    QSqlQuery query(db);
    
    // 执行JOIN查询
    if (!query.exec("SELECT TemporaryUser.username, UsagePeriods.start_time, UsagePeriods.end_time "
                    "FROM TemporaryUser "
                    "JOIN UsagePeriods ON TemporaryUser.id = UsagePeriods.temp_user_id;"))
    {
        qDebug() << "Error: Unable to fetch data!" << query.lastError().text();
        return result;
    }
    
    // 读取查询结果
    while (query.next())
    {
        QString username = query.value(0).toString();  // 获取临时用户的用户名
        QString startTime = query.value(1).toString();  // 获取使用开始时间
        QString endTime = query.value(2).toString();  // 获取使用结束时间
        
        result.append(qMakePair(username, qMakePair(startTime, endTime)));
    }
    return result;
}

QList<QPair<QString, QString> > databaseManager::getUsagePeriodsForTemporaryUser(const QString &userId)
{
    QList<QPair<QString, QString>> result;  // 用于存储该临时用户的所有使用时间段
    QSqlQuery query(db);
    
    // 执行查询，限制为特定临时用户
    query.prepare("SELECT start_time, end_time FROM UsagePeriods WHERE temp_user_id = :userId;");
    query.bindValue(":userId", userId);  // 绑定参数以防止SQL注入
    
    if (!query.exec()) {
        qDebug() << "Error: Unable to fetch data!" << query.lastError().text();
        return result;
    }
    
    // 读取查询结果
    while (query.next()) {
        QString startTime = query.value(0).toString();
        QString endTime = query.value(1).toString();
        
        result.append(qMakePair(startTime, endTime));
    }
    return result;
}

bool databaseManager::insertSysData(const QString &username, const QString &pwd)
{
    QSqlQuery query(db);
    query.prepare("INSERT INTO Sys (username, password) VALUES (?, ?)");
    query.addBindValue(username);
    query.addBindValue(pwd);

    if (!query.exec())
    {
        qWarning() << "Error inserting Sys data:" << query.lastError().text();
        return false;
    }
    return true;
}

bool databaseManager::querySysData(QString &username, QString &pwd)
{
    QSqlQuery query("SELECT * FROM Sys", db);

    if (!query.exec())
    {
        qWarning() << "Error querying Sys data:" << query.lastError().text();
        return false;
    }
    else if(!query.next())
    {
        qDebug() << "Sys data is empty";
        return true;
    }
    else
    {
        username = query.value(1).toString();
        pwd = query.value(2).toString();
        return true;
    }

    return false;
}

bool databaseManager::deleteSysData()
{
    QSqlQuery query(db);
    query.prepare("DELETE FROM Sys");

    if (!query.exec())
    {
        qWarning() << "Error deleting Sys data:" << query.lastError().text();
        return false;
    }
    return true;
}

bool databaseManager::querySysPwd(QString &pwd)
{
    QSqlQuery query("SELECT password FROM Sys", db);

    if (!query.exec())
    {
        qWarning() << "Error querying Sys data:" << query.lastError().text();
        return false;
    }
    else if(!query.next())
    {
        qDebug() << "Sys data is empty";
        return true;
    }
    else
    {
        pwd = query.value(0).toString();
        return true;
    }

    return false;
}

bool databaseManager::insertConfigData(bool tmpUserLogin, QString LoginIdentity)
{
    QSqlQuery query(db);
    query.exec("SELECT COUNT(*) FROM Configuration");
    if (query.next() && query.value(0).toInt() == 0)
    {
        query.prepare("INSERT INTO Configuration (TempUserLogin, LoginIdentity, SaveFilePath) VALUES (?, ?, ?)");
        query.addBindValue(static_cast<int>(tmpUserLogin)); // 默认允许临时用户登录
        query.addBindValue(LoginIdentity); // 默认上次登录身份
        query.addBindValue(QCoreApplication::applicationDirPath() + "/config"); // 默认文件存储路径
        if (!query.exec())
        {
            qDebug() << "Error: Unable to insert default Configuration!" << query.lastError().text();
            return false;
        }
    }
    return true;
}

QString databaseManager::querySaveFilePath()
{
    QSqlQuery query("SELECT SaveFilePath FROM Configuration LIMIT 1", db);
    if (query.next())
        return query.value(0).toString();
    return ""; // 默认值
}

bool databaseManager::queryTempUserLogin()
{
    QSqlQuery query("SELECT TempUserLogin FROM Configuration LIMIT 1", db);
    if (query.next())
    {
        return query.value(0).toInt() == 1;
    }
    return false; // 默认返回 false
}

QString databaseManager::queryLoginIdentity()
{
    QSqlQuery query("SELECT LoginIdentity FROM Configuration LIMIT 1", db);
    if (query.next())
        return query.value(0).toString();
    return ""; // 默认值
}

bool databaseManager::updateTempUserLogin(bool tmpUserLogin)
{
    QSqlQuery query(db);
    query.prepare("UPDATE Configuration SET TempUserLogin = ? WHERE id = 1");
    query.addBindValue(tmpUserLogin);
    
    if (!query.exec())
    {
        qWarning() << "Error updating Configuration tmpUserLogin data:" << query.lastError().text();
        return false;
    }
    return true;
}

bool databaseManager::updateLoginIdentity(QString LoginIdentity)
{
    QSqlQuery query(db);
    query.prepare("UPDATE Configuration SET LoginIdentity = ? WHERE id = 1");
    query.addBindValue(LoginIdentity);
    
    if (!query.exec())
    {
        qWarning() << "Error updating Configuration LoginIdentity data:" << query.lastError().text();
        return false;
    }
    return true;
}

bool databaseManager::updateLastLogoutTime()
{
    QSqlQuery query(db);
    
    query.prepare("UPDATE LoginLog SET logout_time = :logout_time "
                  "WHERE login_time = (SELECT MAX(login_time) FROM LoginLog)"
                  "AND (logout_time IS NULL OR logout_time = '');");
    query.bindValue(":logout_time", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    
    if (!query.exec())
    {
        qDebug() << "Error: Unable to update logout time!" << query.lastError().text();
        return false;
    }
    
//    return query.numRowsAffected() > 0;
    return true;
}

bool databaseManager::quertUnsyncLog(QList<QList<QString> > &logList)
{
    QSqlQuery query("SELECT * FROM LoginLog WHERE logout_time IS NOT NULL AND synced = 0", db);

    if (!query.exec())
    {
        qWarning() << "Error querying LoginLog data:" << query.lastError().text();
        return false;
    }

    while (query.next())
    {
        QList<QString> log;
        log.append(query.value(0).toString());
        log.append(query.value(1).toString());
        log.append(query.value(2).toString());
        log.append(query.value(3).toString());
        log.append(query.value(4).toString());
        
        logList.append(log);
    }

    return !logList.isEmpty();
}

bool databaseManager::updateLogSyncRecord(QString username, QString loginTime, QString logoutTime)
{
    QSqlQuery query(db);
    
    query.prepare("UPDATE LoginLog SET synced = 1 WHERE username = ? AND login_time = ? AND logout_time = ?");
    query.addBindValue(username);
    query.addBindValue(loginTime);
    query.addBindValue(logoutTime);
    
    if (!query.exec())
    {
        qWarning() << "Error updating LoginLog data:" << query.lastError().text();
        return false;
    }
    return true;
}

bool databaseManager::cleanOldLoginLogs(int daysToKeep)
{
    QSqlQuery query(db);
    QString deleteDate = QDateTime::currentDateTime().addDays(-daysToKeep).toString("yyyy-MM-dd HH:mm:ss");
    
    query.prepare("DELETE FROM LoginLog WHERE login_time < :delete_date");
    query.bindValue(":delete_date", deleteDate);
    
    if (!query.exec()) 
    {
        qDebug() << "Error: Failed to clean old logs!" << query.lastError().text();
        return false;
    }
    
    qDebug() << "Cleaned" << query.numRowsAffected() << "old login records";
    
    // 执行VACUUM优化数据库文件大小
    if (!query.exec("VACUUM"))
    {
        qDebug() << "Warning: VACUUM failed!" << query.lastError().text();
    }
    
    return true;
}

bool databaseManager::getUnsyncedRecord(QList<QList<QString> > &recordsList)
{
    QSqlQuery query("SELECT table_name, record_id, operation_type, operation_time FROM SyncLog WHERE synced = 0", db);
    if (!query.exec())
    {
        qWarning() << "Error querying SyncLog data:" << query.lastError().text();
        return false;
    }

    while (query.next())
    {
        QList<QString> record;
        record.append(query.value(0).toString());
        record.append(query.value(1).toString());
        record.append(query.value(2).toString());
        record.append(query.value(3).toString());
        
        recordsList.append(record);
    }
    
    return true;
}

bool databaseManager::getSimplifiedRecord(QList<QList<QString> > &recordsList)
{
    QSqlQuery query("SELECT * FROM SyncLog WHERE synced = 0 AND id IN ("
                    "SELECT id FROM SyncLog AS s1 "
                    "WHERE synced = 0 AND operation_time = (SELECT MAX(operation_time) "
                    "FROM SyncLog AS s2 "
                    "WHERE s2.record_id = s1.record_id AND s2.synced = 0 ) );", db);
    if (!query.exec())
    {
        qWarning() << "Error querying SyncLog data:" << query.lastError().text();
        return false;
    }

    while (query.next())
    {
        QList<QString> record;
        record.append(query.value(1).toString());
        record.append(query.value(2).toString());
        record.append(query.value(3).toString());
        record.append(query.value(4).toString());
        
        recordsList.append(record);
    }
    
    return true;
}

bool databaseManager::updateRecord(QString recordId)
{
    QSqlQuery query(db);
    query.prepare("UPDATE SyncLog SET synced = 1 WHERE record_id = ?");
    query.addBindValue(recordId);
    
    if (!query.exec())
    {
        qWarning() << "Error updating SyncLog synced data:" << query.lastError().text();
        return false;
    }
    return true;
}
