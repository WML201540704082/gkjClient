#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLocalServer>
#include <QLocalSocket>

#include "checkpwdwidget.h"
#include "firstloginwidget.h"
#include "tmpauthmanagewidget.h"
#include "tmpauthorizewidget.h"
#include "facemanagewidget.h"
#include "detailwidget.h"
#include "timewidget.h"
#include "choosefacewidget.h"
#include "faceloginwidget.h"
#include "pwdloginwidget.h"
#include "./viewModel/myviewmodel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum currentWidget
{
    homepage = 1,
    checkpwdwidget = 2,
    firstloginwidget = 3,
    tmpauthmanagewidget = 4,
    tmpauthwidget = 5,
    facemanagewidget = 6,
    facemanagewidget2 = 7,
    detailwidget = 8,
    detailwidget2 = 9,
    detailwidget3 = 10,
    timewidget = 11,
    choosefacewidget = 12,
    faceloginwidget = 13,
    pwdloginwidget = 14
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
    ///
    /// \brief iniUI 初始化主界面UI
    ///
    void iniUI();
    
    ///
    /// \brief screenAdapter 适配屏幕分辨率
    ///
    void screenAdapter();
    
    ///
    /// \brief iniWidget 初始化widget
    ///
    void iniWidget();
    
    ///
    /// \brief iniViewConnection 初始化view层信号槽
    ///
    void iniViewConnection();
    
    ///
    /// \brief iniViewModel 初始化viewModel层对象及相关信号槽
    ///
    void iniViewModel();
    
    ///
    /// \brief iniCurrentWidget 初始化当前应显示的界面
    ///
    void iniCurrentWidget();
    
    
    ///
    /// \brief handleNewConnection 处理新连接
    ///
    void handleNewConnection();
    
    ///
    /// \brief readCommand 读取命令
    ///
    void readCommand(); 
    
    ///
    /// \brief showCurrentWidget 重新显示当前界面
    /// \param cur
    ///
    void showCurrentWidget(currentWidget cur);
     
    ///
    /// \brief mousePressEvent 重写mousePressEvent和mouseMoveEvent用于实现程序拖动及设置可拖动控件区域
    /// \param event
    ///
    void mousePressEvent(QMouseEvent* event);

    ///
    /// \brief mouseMoveEvent 重写mousePressEvent和mouseMoveEvent用于实现程序拖动及设置可拖动控件区域
    /// \param event
    ///
    void mouseMoveEvent(QMouseEvent* event);
    
public slots:
    
    
private slots:
    void on_pushButton_clicked();
    
    void onRefeshUsername(QString username);
    
    ///
    /// \brief onCheckSuccessful
    ///
    void onCheckSuccessful();
    
    ///
    /// \brief onReceiveSuccess
    ///
    void onReceiveSuccess();
    
    ///
    /// \brief onReregister
    ///
    void onReregister();
    
    ///
    /// \brief onChangeWidget
    ///
    void onChangeWidget();
    
    ///
    /// \brief onReturnHomePage
    ///
    void onReturnHomePage();
    
    ///
    /// \brief onReturnFaceManagePage
    ///
    void onReturnFaceManagePage();
    
    ///
    /// \brief onReturnFaceManagePage2
    ///
    void onReturnFaceManagePage2();
    
    ///
    /// \brief onReturnAuthManagePage
    ///
    void onReturnAuthManagePage();
    
    ///
    /// \brief onAuthorize
    ///
    void onAuthorize();
    
    ///
    /// \brief onFaceManage
    ///
    void onFaceManage();
    
    ///
    /// \brief onReturnTmpAuthManagePage
    ///
    void onReturnTmpAuthManagePage();
    
    ///
    /// \brief onTmpUsrFaceRegister
    ///
    void onTmpUsrFaceRegister();
    
    ///
    /// \brief onLoadTmpUserDetail
    /// \param uuidStr
    /// \param username
    /// \param pixmap
    ///
    void onLoadTmpUserDetail(const QString& uuidStr, const QString& username, QPixmap pixmap);
    
    ///
    /// \brief onChangeFacialInfo
    /// \param tmpUserUuid
    /// \param tmpUsername
    ///
    void onChangeFacialInfo(const QString& tmpUserUuid, const QString& tmpUsername);
    
    ///
    /// \brief onAddTmpUserPeriod
    /// \param tmpUserUuid
    /// \param tmpUsername
    /// \param tmpPixmap
    ///
    void onAddTmpUserPeriod(const QString& tmpUserUuid, const QString& tmpUsername, QPixmap tmpPixmap);
    
    ///
    /// \brief onChangeTmpUserPeriod
    /// \param uuidStr
    /// \param tmpUsername
    /// \param tmpPixmap
    /// \param startTime
    /// \param endTime
    ///
    void onChangeTmpUserPeriod(const QString& uuidStr, const QString& tmpUsername, QPixmap tmpPixmap,
                               const QString& startTime, const QString& endTime);
    
    ///
    /// \brief onChangeTmpUserPeriod2
    /// \param uuidStr
    /// \param tmpUsername
    /// \param tmpPixmap
    /// \param startTime
    /// \param endTime
    ///
    void onChangeTmpUserPeriod2(const QString& uuidStr, const QString& tmpUsername, QPixmap tmpPixmap,
                                const QString& startTime, const QString& endTime);

    ///
    /// \brief onReturnDetailPage
    ///
    void onReturnDetailPage();
    
    ///
    /// \brief onReturnDetailPage2
    ///
    void onReturnDetailPage2();
    
    ///
    /// \brief onAddUserPeriodSuccess
    ///
    void onAddUserPeriodSuccess();
    
    ///
    /// \brief onChangeUserPeriodSuccess
    ///
    void onChangeUserPeriodSuccess();
    
    ///
    /// \brief onChangeUserPeriodSuccess2
    ///
    void onChangeUserPeriodSuccess2();
    
    ///
    /// \brief onSendTmpUserFaceInfo
    /// \param uuidStr
    /// \param username
    /// \param pixmap
    ///
    void onSendTmpUserFaceInfo(const QString& uuidStr, const QString& username, QPixmap pixmap);
    
    ///
    /// \brief onReturnFirstLoginPage
    ///
    void onReturnTmpAuthPage();
    
    ///
    /// \brief onChooseFace
    ///
    void onChooseFace();
    
    ///
    /// \brief onSetConfigPwdSuccess
    ///
    void onSetConfigPwdSuccess();
    
    ///
    /// \brief onCheckPassed
    ///
    void onCheckPassed();
    
    ///
    /// \brief onShowPwdLoginPage
    ///
    void onShowPwdLoginPage();
    
    ///
    /// \brief onReturnFaceLoginPage
    ///
    void onReturnFaceLoginPage();
    
    void on_pushButton_2_clicked();
    
    void on_btn_min_clicked();
    
    void on_btn_close_clicked();
    
signals:
    void iniDatabase();
    void checkUsername();
    void reregister();
    void showReturnBtn();
    void reregisterCheckPwd();
    void iniCheckThread();
    void iniSyncThread();
    
private:
    Ui::MainWindow *ui;
    
    QPoint whereismouse;
    
    currentWidget cur;
    
    checkPwdWidget *pCheckPwdWidget;
    firstLoginWidget *pFirstLoginWidget;
    tmpAuthManageWidget *pTmpAuthManageWidget;
    tmpAuthorizeWidget *pTmpAuthWidget;
    faceManageWidget *pFaceManageWidget;
    detailWidget *pDetailWidget;
    timeWidget *pTimeWidget;
    chooseFaceWidget *pChooseFaceWidget;
    faceLoginWidget *pFaceLoginWidget;
    pwdLoginWidget *pPwdLoginWidget;
    
    myViewModel *viewModel = nullptr;
    
    QLocalServer *server;
    QLocalSocket *clientSocket;
    
    bool adminLogin = false;
    bool firstLogin = true;
};
#endif // MAINWINDOW_H
