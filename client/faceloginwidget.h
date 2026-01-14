#ifndef FACELOGINWIDGET_H
#define FACELOGINWIDGET_H

#include <QWidget>

namespace Ui {
class faceLoginWidget;
}

class faceLoginWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit faceLoginWidget(QWidget *parent = nullptr);
    ~faceLoginWidget();
    
    ///
    /// \brief iniUI 初始化UI界面
    ///
    void iniUI();
    
    ///
    /// \brief screenAdapt 适配分辨率
    ///
    void screenAdapt();
    
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
    /// \brief iniCamera 初始化相机
    ///
    void iniCamera();
    
    ///
    /// \brief openCamera 打开相机
    ///
    void openCamera();
    
    ///
    /// \brief showFloatingErrorWindow 显示错误信息悬浮窗
    /// \param str
    ///
    void showFloatingErrorWindow(QString str);
    
public slots:
    ///
    /// \brief onRepostCamMessage 显示相机信息
    /// \param message
    ///
    void onRepostCamMessage(QString message);
    
    ///
    /// \brief onSendVmMessage 显示vm层信息
    /// \param message
    ///
    void onSendVmMessage(QString message);
    
    ///
    /// \brief onSendPixmap 显示人脸图片
    /// \param pixmap
    ///
    void onSendPixmap(QPixmap pixmap);
    
    ///
    /// \brief onSendUserIdentity 处理用户身份槽函数
    /// \param identityStr
    ///
    void onSendUserIdentity(QString identityStr);
    
private slots:
    void on_btn_pwd_clicked();
    
    void on_btn_img_clicked();
    
    void on_btn_min_clicked();
    
    void on_btn_close_clicked();
    
signals:
    void iniCam();
    void openCam(HWND m_hWnd, int nVideoWindowWidth, int nVideoWindowHeight);
    void startDetection();
    void checkUserFace();
    void checkPassed();
    void showPwdLoginPage();
    void uniniCam();
    
private:
    Ui::faceLoginWidget *ui;
    
    QPoint whereismouse;
    
    bool camIni = false;
    bool timeout = false;
};

#endif // FACELOGINWIDGET_H
