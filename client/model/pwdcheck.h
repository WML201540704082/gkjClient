#ifndef PWDCHECK_H
#define PWDCHECK_H

#include <QObject>

#include <windows.h>
#include <lmcons.h>
#include <winbase.h>

class pwdCheck : public QObject
{
    Q_OBJECT
public:
    explicit pwdCheck(QObject *parent = nullptr);
    
    BOOL ValidatePassword(const std::wstring& username, const std::wstring& password);
    
signals:
    
};

#endif // PWDCHECK_H
