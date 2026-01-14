#ifndef FIRSTLOGINWIDGET_H
#define FIRSTLOGINWIDGET_H

#include "dvtcamera.h"

#include <QWidget>
#include <QPixmap>

namespace Ui {
class firstLoginWidget;
}

enum registerType
{
    admin = 1,
    tmpUser = 2,
    changeFace = 3
};

class firstLoginWidget : public QWidget
{
    Q_OBJECT

public:
    explicit firstLoginWidget(QWidget *parent = nullptr);
    ~firstLoginWidget();

    ///
    /// \brief iniUI 初始化UI界面
    ///
    void iniUI();

    registerType type = admin;
    QString curUuid;
    QString curUsername;
    
    ///
    /// \brief screenAdapt 适配分辨率
    ///
    void screenAdapt();

    ///
    /// \brief iniCamera 初始化相机
    ///
    void iniCamera();
    
    ///
    /// \brief openCamera 打开相机
    ///
    void openCamera();
    
    ///
    /// \brief onShowReturnBtn
    ///
    void onShowReturnBtn();
    
    ///
    /// \brief showFloatingErrorWindow 显示错误信息悬浮窗
    /// \param str
    ///
    void showFloatingErrorWindow(QString str);
    
    
    
    ///
    /// \brief mousePressEvent 重写mousePressEvent和mouseMoveEvent用于实现程序拖动及设置可拖动控件区域
    /// \param event
    ///
    void mousePressEvent(QMouseEvent* event) override;

    ///
    /// \brief mouseMoveEvent 重写mousePressEvent和mouseMoveEvent用于实现程序拖动及设置可拖动控件区域
    /// \param event
    ///
    void mouseMoveEvent(QMouseEvent* event) override;
    
    ///
    /// \brief keyPressEvent
    /// \param event
    ///
    void keyPressEvent(QKeyEvent *event) override;
    
    ///
    /// \brief setUsername 变更信息时设置用户名
    /// \param username
    ///
    void setUsername(QString username);
       
private slots:
    void on_btn_register_clicked();

    void on_btn_img_clicked();

    void on_btn_refresh_clicked();
    
    void on_btn_return_clicked();
    
    void on_btn_min_clicked();
    
    void on_btn_close_clicked();
    
public slots:
    ///
    /// \brief onRepostCamMessage 显示相机信息
    /// \param messgae
    ///
    void onRepostCamMessage(QString messgae);
    
    ///
    /// \brief onSendPixmap 显示人脸图片
    /// \param pixmap
    ///
    void onSendPixmap(QPixmap pixmap);
    
    ///
    /// \brief onSendVmMessage 显示vm层信息
    /// \param message
    ///
    void onSendVmMessage(QString message);
    
    ///
    /// \brief onSendRegisterResult 显示注册结果
    /// \param result
    ///
    void onSendRegisterResult(bool result);
    
    ///
    /// \brief onSendTmpRegisterResult2 处理临时用户注册结果
    /// \param result
    /// \param str
    ///
    void onSendTmpRegisterResult2(bool result, QString str);

    ///
    /// \brief onSendUpdateTmpUsrInfoRes 处理更新临时用户信息结果
    /// \param result
    /// \param newUsername
    ///
    void onSendUpdateTmpUsrInfoRes(bool result, QString newUsername);
    
signals:
    void success();
    void iniCam();
    void uniniCam();
    void openCam(HWND m_hWnd, int nVideoWindowWidth, int nVideoWindowHeight);
    void startDetection();
    void userRegistration(QString user, QString pwd, QString department);
    void savePwd();
    void userReregistration(QString user, QString pwd, QString department);
    void returnHomePage();
    void returnFaceManagePage();
    void returnFaceManagePage2();
    void tmpUserRegistration(QString tmpUser, QString pwd, QString department);
    void returnDetailPage();
    void returnDetailPage2();
    void changeTmpUserInfo(QString tmpUserUuid, QString tmpUsername, QString tmpUserPwd, QString department);
    void delTmpUserEncFile(QString username);
    void startSync();

private:
    Ui::firstLoginWidget *ui;
    
    QPoint whereismouse;
    
    bool detectionStatus = false;
};

#endif // FIRSTLOGINWIDGET_H
