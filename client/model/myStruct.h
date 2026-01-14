#ifndef MYSTRUCT_H
#define MYSTRUCT_H

#include <QString>

struct adminInfo
{
    QString type;
    QString id;
    QString username;
    QString userPwd;
    QString department;
    QString faceData;
    QString operationTime;
};

struct unsyncAdminInfo
{
    adminInfo admin;
    QString errorType; //1：信息不全；2：管理员表已经存在数据 3：加密人脸数据失败 4：加密密码失败 5：存储失败
};

struct usageTime
{
    QString type;
    QString id;
    QString userId;
    QString startTime;
    QString endTime;
    QString operationTime;
};

struct unsyncUsageTime
{
    usageTime time;
    QString errorType;  // 1:信息不全； 2：更新信息失败；3：添加信息失败
};

struct tmpUserInfo
{
    QString type;
    QString id;
    QString username;
    QString userPwd;
    QString department;
    QString faceData;
    QString operationTime;
};

struct unsyncTmpUserInfo
{
    tmpUserInfo tmpUser;
    unsigned int errorType; // 1:信息不全； 2：与管理员人脸比对失败；3：加密人脸数据失败；
                            //4：加密密码失败；5：添加用户数据失败；6：更新用户数据失败；7:删除用户数据失败；
};

struct loginInfo
{
    QString username;
    QString loginTime;
    QString logoutTime;
    QString department;
};

#endif // MYSTRUCT_H
