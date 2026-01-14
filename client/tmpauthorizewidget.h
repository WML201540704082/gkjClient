#ifndef TMPAUTHORIZEWIDGET_H
#define TMPAUTHORIZEWIDGET_H

#include <QWidget>

namespace Ui {
class tmpAuthorizeWidget;
}

class tmpAuthorizeWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit tmpAuthorizeWidget(QWidget *parent = nullptr);
    ~tmpAuthorizeWidget();
    
    void iniUI();
    
    void screenAdapter();
    
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
    /// \brief changeEvent 解决开启相机后点击返回再切回该界面后ui控件无反应的问题
    /// \param event
    ///
    void changeEvent(QEvent *event) override;
    
    ///
    /// \brief showFloatingErrorWindow 显示错误信息悬浮窗
    /// \param str
    ///
    void showFloatingErrorWindow(QString str);
    
    ///
    /// \brief iniCamera 初始化相机
    ///
    void iniCamera();
    
    ///
    /// \brief openCamera 打开相机
    ///
    void openCamera();
    
    ///
    /// \brief setPixmap 选择人脸后设置图片
    /// \param pixmap
    ///
    void setPixmap(QPixmap pixmap);
    
    ///
    /// \brief setUsername 设置用户名
    /// \param username
    ///
    void setUsername(QString username);
    
public slots:
    ///
    /// \brief onSendPixmap 显示人脸图片
    /// \param pixmap
    ///
    void onSendPixmap(QPixmap pixmap);
    
    ///
    /// \brief onRepostCamMessage 显示相机信息
    /// \param messgae
    ///
    void onRepostCamMessage(QString message);
    
    ///
    /// \brief onSendVmMessage 显示vm层信息
    /// \param message
    ///
    void onSendVmMessage(QString message);
    
    ///
    /// \brief onSendTmpRegisterResult 显示临时用户注册结果
    /// \param result
    /// \param str
    ///
    void onSendTmpRegisterResult(bool result, QString str);
    
private slots:
    void on_pushButton_clicked();
    
    void on_pushButton_3_clicked();
    
    void on_btn_min_clicked();
    
    void on_btn_close_clicked();
    
    void on_btn_return_clicked();
    
    void on_pushButton_2_clicked();
    
signals:
    void iniCam();
    void uniniCam();
    void openCam(HWND m_hWnd, int nVideoWindowWidth, int nVideoWindowHeight);
    void startDetection();
    void tmpUserRegistration(QString username, QString userPwd, QString startTime, QString endTime, QString department);
    void returnAuthManagePage();
    void chooseFace();
    void startSync();
    
private:
    Ui::tmpAuthorizeWidget *ui;
    
    QPoint whereismouse;
    
    bool detectionStatus = false;
    
    bool camIni = false;
    bool camOpen = false;
    bool timeout = false;
};

#endif // TMPAUTHORIZEWIDGET_H
