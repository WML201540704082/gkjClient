#include "pwdcheck.h"
#include "utf8.h"

pwdCheck::pwdCheck(QObject *parent) : QObject(parent)
{
    
}

BOOL pwdCheck::ValidatePassword(const std::wstring &username, const std::wstring &password)
{
    HANDLE hToken = NULL;
    BOOL result = LogonUser(
        username.c_str(), // 用户名
        L".",              // 域名（本地计算机使用".")
        password.c_str(),  // 密码
        LOGON32_LOGON_INTERACTIVE, // 登录类型
        LOGON32_PROVIDER_DEFAULT, // 提供者
        &hToken);          // 返回的句柄

    if (result) 
    {
        // 成功登录
        CloseHandle(hToken);  // 使用完毕后需要关闭句柄
        return TRUE;
    } 
    else 
    {
        // 登录失败
        return FALSE;
    }
}
