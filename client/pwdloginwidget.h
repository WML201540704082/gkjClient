#ifndef PWDLOGINWIDGET_H
#define PWDLOGINWIDGET_H

#include <QWidget>

namespace Ui {
class pwdLoginWidget;
}

class pwdLoginWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit pwdLoginWidget(QWidget *parent = nullptr);
    ~pwdLoginWidget();
    
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
    /// \brief keyPressEvent 重写keyPressEvent实现回车触发按钮点击事件
    /// \param event
    ///
    void keyPressEvent(QKeyEvent *event) override;
    
    ///
    /// \brief iniUi 初始化UI界面
    ///
    void iniUi();
    
    ///
    /// \brief screenAdpater 适配屏幕分辨率
    ///
    void screenAdpater();
    
    ///
    /// \brief showFloatingErrorWindow 显示错误信息悬浮窗
    /// \param str
    ///
    void showFloatingErrorWindow(QString str);
    
public slots:
    void onSendPwdCheckResult(bool result);
    
private slots:
    void on_btn_return_clicked();
    
    void on_btn_min_clicked();
    
    void on_btn_close_clicked();
    
    void on_btn_clicked();
    
signals:
    void returnFaceLoginPage();
    void checkUserPwd(QString pwdStr);
    void checkPassed();
    
private:
    Ui::pwdLoginWidget *ui;
    
    QPoint whereismouse;
};

#endif // PWDLOGINWIDGET_H
