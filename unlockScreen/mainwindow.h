#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "./viewModel/myviewmodel.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
    void iniUI();
    void screenAdapter();
    
    ///
    /// \brief iniViewModel 初始化viewModel层对象及相关信号槽
    ///
    void iniViewModel();
    
    ///
    /// \brief needSync 判断是否可以同步
    ///
    void needSync();
    
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
    /// \param messgae
    ///
    void onRepostCamMessage(QString messgae);
    
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
    /// \brief onSendAvailableUserInfo 显示可用人员信息
    /// \param adminUsername
    /// \param adminPwd
    /// \param adminPixmap
    ///
    void onSendAvailableUserInfo(QString adminUsername, QString adminPwd, QPixmap adminPixmap, QString adminDepartment);
    
    ///
    /// \brief onSendAvailableUserInfo2 显示可用人员信息
    /// \param adminUsername
    /// \param adminPwd
    /// \param adminPixmap
    /// \param tmpUserInfo
    ///
    void onSendAvailableUserInfo2(QString adminUsername, QString adminPwd, QPixmap adminPixmap, QString adminDepartment,
                                  QList<std::tuple<QString, QString, QPixmap, QString>> tmpUserInfo);
private slots:
    void on_btn_img_clicked();
    
    void on_btn_refresh_clicked();
    
    void on_btn_pwdlogin_clicked();
    
    void on_btn_userpwdlogin_clicked();
    
    void on_comboBox_currentIndexChanged(int index);
    
    void on_btn_pwdlogin_2_clicked();
    
signals:
    void iniDatabase();
    void checkAdminData();
    void iniCam();
    void openCam(HWND m_hWnd, int nVideoWindowWidth, int nVideoWindowHeight);
    void startDetection();
    void facialAuthenticationPassed();
    void keyboardLogin();
    void pwdLogin(QString identity, QString username, QString department);
    
    void iniSyncThread();
    
private:
    Ui::MainWindow *ui;
    
    myViewModel *viewModel = nullptr;
    
    QString usernameAdmin;
    QString pwdAdmin;
    QPixmap pixmapAdmin;
    QString departmentAdmin;
    QList<std::tuple<QString, QString, QPixmap, QString> > tmpUserList;
    
    double w_fx = 0; // 横向缩放因子
    double h_fx = 0; // 纵向缩放因子
};
#endif // MAINWINDOW_H
