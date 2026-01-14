#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utf8.h"
#include "floatingerrorwindow.h"

#include <QAction>
#include <QScreen>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    showFullScreen();  
    iniUI();
    screenAdapter();
    
    iniViewModel();
    
    needSync();
    
//    qDebug()<<"emit checkAdminData()";
//    emit checkAdminData();
    iniCamera();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::iniUI()
{
    QAction *action = new QAction(this);
    action->setIcon(QIcon(":/images/images/cam.png"));
    ui->lineEdit_status->addAction(action,QLineEdit::LeadingPosition);
    ui->lineEdit_status->setText("相机未初始化");
    
    ui->stackedWidget->setCurrentIndex(0);
    ui->stackedWidget_2->setCurrentIndex(0);
}

void MainWindow::screenAdapter()
{
    QScreen* pScreen = QGuiApplication::primaryScreen();
    QRect rect1 = pScreen->geometry();

    w_fx = (double)rect1.size().width() / 1920.0;
    h_fx = (double)rect1.size().height() / 1080.0;
    
    ui->verticalLayout_2->setContentsMargins(60 * w_fx, 60 * h_fx, 60 * w_fx, 9 * h_fx);
    ui->verticalLayout_2->setSpacing(30 * h_fx);
    
    ui->frame->setMinimumSize(1800 * w_fx, 76 * h_fx);
    ui->frame->setMaximumSize(1800 * w_fx, 76 * h_fx);
    
    ui->label->setMinimumSize(193 * w_fx, 76 * h_fx);
    ui->label->setMaximumSize(193 * w_fx, 76 * h_fx);
    
    ui->gridLayout_2->setVerticalSpacing(67 * h_fx);
    
    ui->label_2->setMinimumSize(597 * w_fx, 88 * h_fx);
    ui->label_2->setMaximumSize(597 * w_fx, 88 * h_fx);
    
    ui->frame_3->setMinimumSize(620 * w_fx, 500 * h_fx);
    ui->frame_3->setMaximumSize(620 * w_fx, 500 * h_fx);
    
    ui->stackedWidget_2->setMinimumSize(600 * w_fx, 400 * h_fx);
    ui->stackedWidget_2->setMaximumSize(600 * w_fx, 400 * h_fx);
    
    ui->verticalLayout->setContentsMargins(9 * w_fx, 0 * h_fx, 9 * w_fx, 9 * h_fx);
    ui->verticalLayout->setSpacing(16 * h_fx);
    
    ui->frame_4->setMinimumSize(600 * w_fx, 32 * h_fx);
    ui->frame_4->setMaximumSize(600 * w_fx, 32 * h_fx);
    
    ui->horizontalLayout_2->setSpacing(6 * w_fx);
    
    ui->label_3->setMinimumSize(100 * w_fx, 30 * h_fx);
    ui->label_3->setMaximumSize(100 * w_fx, 30 * h_fx);  
    QFont font = ui->label_3->font();
    font.setPointSize(12 * w_fx);
    ui->label_3->setFont(font);
    
    ui->lineEdit_status->setMinimumSize(346 * w_fx, 30 * h_fx);
    ui->lineEdit_status->setMaximumSize(346 * w_fx, 30 * h_fx);
    
    ui->btn_refresh->setMinimumSize(30 * w_fx, 30 * h_fx);
    ui->btn_refresh->setMaximumSize(30 * w_fx, 30 * h_fx);
    
    ui->frame_5->setMinimumSize(600 * w_fx, 358 * h_fx);
    ui->frame_5->setMaximumSize(600 * w_fx, 358 * h_fx);
    
    ui->gridLayout_3->setContentsMargins(9 * w_fx, 0 * h_fx, 9 * w_fx, 24 * h_fx);
    ui->gridLayout_3->setVerticalSpacing(16 * h_fx);
    
    ui->stackedWidget->setMinimumSize(240 * w_fx, 239 * h_fx);
    ui->stackedWidget->setMaximumSize(240 * w_fx, 239 * h_fx);
    
    ui->page1->setMinimumSize(240 * w_fx, 239 * h_fx);
    ui->page1->setMaximumSize(240 * w_fx, 239 * h_fx);
    
    ui->label_4->setMinimumSize(240 * w_fx, 32 * h_fx);
    ui->label_4->setMaximumSize(240 * w_fx, 32 * h_fx);
    font = ui->label_4->font();
    font.setPointSize(10 * w_fx);
    ui->label_4->setFont(font);
    
    ui->btn_img->setMinimumSize(240 * w_fx, 239 * w_fx);
    ui->btn_img->setMaximumSize(240 * w_fx, 239 * w_fx);
    font = ui->btn_img->font();
    font.setPointSize(10 * w_fx);
    ui->btn_img->setFont(font);
    
    ui->page2->setMinimumSize(240 * w_fx, 239 * h_fx);
    ui->page2->setMaximumSize(240 * w_fx, 239 * h_fx);
    
    ui->page3->setMinimumSize(240 * w_fx, 239 * h_fx);
    ui->page3->setMaximumSize(240 * w_fx, 239 * h_fx);
    
    ui->label_5->setMinimumSize(240 * w_fx, 224 * h_fx);
    ui->label_5->setMaximumSize(240 * w_fx, 224 * h_fx);
    
    ui->frame_6->setMinimumSize(600 * w_fx, 64 * h_fx);
    ui->frame_6->setMaximumSize(600 * w_fx, 64 * h_fx);
    
    ui->page->setMinimumSize(600 * w_fx, 400 * h_fx);
    ui->page->setMaximumSize(600 * w_fx, 400 * h_fx);
    
    ui->verticalLayout_3->setSpacing(10 * h_fx);
    
    ui->page_2->setMinimumSize(600 * w_fx, 400 * h_fx);
    ui->page_2->setMaximumSize(600 * w_fx, 400 * h_fx);
    
    ui->frame_8->setMinimumSize(600 * w_fx, 400 * h_fx);
    ui->frame_8->setMaximumSize(600 * w_fx, 400 * h_fx);
    
    ui->frame_9->setMinimumSize(600 * w_fx, 117 * h_fx);
    ui->frame_9->setMaximumSize(600 * w_fx, 117 * h_fx);
    
    ui->gridLayout_7->setHorizontalSpacing(6 * w_fx);
    ui->gridLayout_7->setVerticalSpacing(6 * w_fx);
    
    ui->label_6->setMinimumSize(240 * w_fx, 240 * h_fx);
    ui->label_6->setMaximumSize(240 * w_fx, 240 * h_fx);
    
    ui->label_account->setMinimumSize(50 * w_fx, 22 * h_fx);
    ui->label_account->setMaximumSize(50 * w_fx, 22 * h_fx);
    font = ui->label_account->font();
    font.setPointSize(8 * w_fx);
    ui->label_account->setFont(font);
    
    ui->label_pwd->setMinimumSize(50 * w_fx, 22 * h_fx);
    ui->label_pwd->setMaximumSize(50 * w_fx, 22 * h_fx);
    font = ui->label_pwd->font();
    font.setPointSize(8 * w_fx);
    ui->label_pwd->setFont(font);
    
    ui->comboBox->setMinimumSize(240 * w_fx, 22 * h_fx);
    ui->comboBox->setMaximumSize(240 * w_fx, 22 * h_fx);
    font = ui->comboBox->font();
    font.setPointSize(8 * w_fx);
    ui->comboBox->setFont(font);
    
    ui->lineEdit_pwd->setMinimumSize(240 * w_fx, 22 * h_fx);
    ui->lineEdit_pwd->setMaximumSize(240 * w_fx, 22 * h_fx);
    font = ui->lineEdit_pwd->font();
    font.setPointSize(8 * w_fx);
    ui->lineEdit_pwd->setFont(font);
    
    ui->btn_pwdlogin_2->setMinimumSize(50 * w_fx, 22 * h_fx);
    ui->btn_pwdlogin_2->setMaximumSize(50 * w_fx, 22 * h_fx);
    
    ui->btn_userpwdlogin->setMaximumSize(206 * w_fx, 48 * h_fx);
    ui->btn_userpwdlogin->setMinimumSize(206 * w_fx, 48 * h_fx);
    font = ui->btn_userpwdlogin->font();
    font.setPointSize(10 * w_fx);
    ui->btn_userpwdlogin->setFont(font);
    
    ui->horizontalLayout_3->setSpacing(16 * h_fx);
    
    ui->frame_7->setMinimumSize(620 * w_fx, 64 * h_fx);
    ui->frame_7->setMaximumSize(620 * w_fx, 64 * h_fx);
    
    ui->btn_pwdlogin->setMaximumSize(80 * w_fx, 22 * h_fx);
    ui->btn_pwdlogin->setMinimumSize(80 * w_fx, 22 * h_fx);
    font = ui->btn_pwdlogin->font();
    font.setPointSize(6 * w_fx);
    ui->btn_pwdlogin->setFont(font);
    
    ui->horizontalLayout_5->setSpacing(16 * h_fx);
}


void MainWindow::iniViewModel()
{
    viewModel = new myViewModel();
    
    connect(this, &MainWindow::iniDatabase, viewModel, &myViewModel::onIniDatabase);
    
    connect(this, &MainWindow::checkAdminData, viewModel, &myViewModel::onCheckAdminData);
    connect(viewModel, &myViewModel::checkThreadErrorFinished, [](QString errStr){
        QMessageBox *msgBox = new QMessageBox(QMessageBox::Warning, "error", errStr, QMessageBox::Ok);
        QObject::connect(msgBox, &QMessageBox::buttonClicked, [](){
            QApplication::quit();
        });
        msgBox->show();
    });
    
    connect(this, &MainWindow::iniCam, viewModel, &myViewModel::onIniCam);
    connect(this, &MainWindow::openCam, viewModel, &myViewModel::onOpenCam);
    connect(this, &MainWindow::startDetection, viewModel, &myViewModel::onStartDetection);
    connect(this, &MainWindow::facialAuthenticationPassed, 
            viewModel, &myViewModel::onFacialAuthenticationPassed);
    connect(viewModel, &myViewModel::repostCamMessage, this, &MainWindow::onRepostCamMessage);
    connect(viewModel, &myViewModel::sendPixmap, this, &MainWindow::onSendPixmap);
    connect(viewModel, &myViewModel::sendVmMessage, this, &MainWindow::onSendVmMessage);
    connect(this, &MainWindow::keyboardLogin, viewModel, &myViewModel::onKeyboardLogin);
    connect(viewModel, &myViewModel::sendAvailableUserInfo, this, &MainWindow::onSendAvailableUserInfo);
    connect(viewModel, &myViewModel::sendAvailableUserInfo2, this, &MainWindow::onSendAvailableUserInfo2);
    connect(this, &MainWindow::pwdLogin, viewModel, &myViewModel::onPwdLogin);
    connect(this, &MainWindow::iniSyncThread, viewModel, &myViewModel::onIniSyncThread);
    
    emit iniDatabase();
}

void MainWindow::needSync()
{
    bool firstLogin = viewModel->isFirstLogin();
    bool checkPwd = viewModel->isPwdChecked();
    if(!checkPwd && firstLogin)
    {
        emit checkAdminData();
    }
    else if(checkPwd && !firstLogin)
    {
        emit iniSyncThread();
    }
}

void MainWindow::iniCamera()
{
    qDebug()<<"iniCamera()";
    emit iniCam();
}

void MainWindow::openCamera()
{
    qDebug()<<"openCamera()";
    emit openCam((HWND)ui->widget_cam->winId(), 
                 ui->widget_cam->width(), ui->widget_cam->height());
    
    qDebug()<<"emit startDetection()";
    emit startDetection();
}

void MainWindow::showFloatingErrorWindow(QString str)
{
    // 获取主窗口的 frameGeometry（包括边框和标题栏的绝对位置）
    QRect mainWindowRect = this->frameGeometry();     
    
    // 计算主窗口的中心位置
    int mainWindowCenterX = mainWindowRect.center().x();  // 主窗口中心 X 坐标
    int mainWindowTopY = mainWindowRect.top();  // 主窗口顶部 Y 坐标
    
    // 创建并显示错误窗口
    floatingErrorWindow* errorWindow = new floatingErrorWindow(str, nullptr);
    
    // 计算错误窗口的新位置，放置在主窗口的中心偏上
    int offsetFromTop = 50;  // 偏移量，可根据需要调整
    int errorWindowX = mainWindowCenterX - errorWindow->width() / 2;  // 水平居中
    int errorWindowY = mainWindowTopY + offsetFromTop;  // 垂直靠近顶部
    
    // 将错误窗口移动到计算出的相对坐标
    errorWindow->move(errorWindowX, errorWindowY);
    
    // 显示错误窗口
    errorWindow->show();
}

void MainWindow::onRepostCamMessage(QString messgae)
{
    ui->lineEdit_status->setText(messgae);
}

void MainWindow::onSendVmMessage(QString message)
{
    if(message != "处理人脸图片成功")
    {
        showFloatingErrorWindow(message);
    }   
}

void MainWindow::onSendPixmap(QPixmap pixmap)
{
    ui->stackedWidget->setCurrentIndex(2);
    
    ui->label_5->setPixmap(pixmap);
    ui->label_5->setScaledContents(true);
    
    emit facialAuthenticationPassed();
}

void MainWindow::onSendAvailableUserInfo(QString adminUsername, QString adminPwd, QPixmap adminPixmap, QString adminDepartment)
{
    usernameAdmin = adminUsername;
    pwdAdmin = adminPwd;
    pixmapAdmin = adminPixmap;
    departmentAdmin = adminDepartment;
    
    ui->comboBox->addItem(usernameAdmin);
}

void MainWindow::onSendAvailableUserInfo2(QString adminUsername, QString adminPwd, QPixmap adminPixmap, QString adminDepartment, QList<std::tuple<QString, QString, QPixmap, QString> > tmpUserInfo)
{
    usernameAdmin = adminUsername;
    pwdAdmin = adminPwd;
    pixmapAdmin = adminPixmap;
    departmentAdmin = adminDepartment;
    tmpUserList = tmpUserInfo;
    
    ui->comboBox->addItem(usernameAdmin);
    for(const std::tuple<QString, QString, QPixmap, QString> &tmpUser : tmpUserList)
    {
        QString tmpUsername = std::get<0>(tmpUser);
        ui->comboBox->addItem(tmpUsername);
    }
}

void MainWindow::on_btn_img_clicked()
{
    if(ui->lineEdit_status->text() != "相机初始化成功")
    {
        return;
    }
    ui->stackedWidget->setCurrentIndex(1);
    
//    iniCamera();
    openCamera();
}

void MainWindow::on_btn_refresh_clicked()
{
    if(ui->lineEdit_status->text() == "相机未初始化")
    {
        iniCamera();
        openCamera();
    }
    else if(ui->lineEdit_status->text() == "检测超时，请点击按钮重试！")
    {
        ui->lineEdit_status->setText("相机初始化成功");
        emit startDetection();
    }
}

void MainWindow::on_btn_pwdlogin_clicked()
{
    emit keyboardLogin();
    QApplication::quit();
}

void MainWindow::on_btn_userpwdlogin_clicked()
{
    if(ui->stackedWidget_2->currentIndex() == 0)
    {
        ui->btn_userpwdlogin->setText("用户人脸登录");
        ui->stackedWidget_2->setCurrentIndex(1);
    }
    else if(ui->stackedWidget_2->currentIndex() == 1)
    {
        ui->btn_userpwdlogin->setText("用户密码登录");
        ui->stackedWidget_2->setCurrentIndex(0);
    }
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    QString username = ui->comboBox->itemText(index);
    if(username == usernameAdmin)
    {
        QPixmap tmpPixmap = pixmapAdmin.scaled(ui->label_6->width(), ui->label_6->height(), 
                            Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        ui->label_6->setPixmap(tmpPixmap);
        ui->label_6->setScaledContents(true);
    }
    else
    {
        for(const std::tuple<QString, QString, QPixmap, QString> &tmpUser : tmpUserList)
        {
            QString tmpUsername = std::get<0>(tmpUser);
            if(username != tmpUsername)
            {
                continue;
            }
            QPixmap tmpPixmap = std::get<2>(tmpUser).scaled(ui->label_6->width(), ui->label_6->height(), 
                                Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            ui->label_6->setPixmap(tmpPixmap);
            ui->label_6->setScaledContents(true);
        }
    }
}

void MainWindow::on_btn_pwdlogin_2_clicked()
{
    QString userPwd = ui->lineEdit_pwd->text();
    QString username = ui->comboBox->currentText();
    
    qDebug()<<"username "<<username;
    qDebug()<<"userPwd "<<userPwd;
    qDebug()<<"usernameAdmin "<<usernameAdmin;
    qDebug()<<"pwdAdmin "<<pwdAdmin;
    
    
    if(username.isEmpty() || userPwd.isEmpty())
    {
        return;
    }
    
    if(username == usernameAdmin && userPwd == pwdAdmin)
    {
        qDebug()<<"username == usernameAdmin && userPwd == pwdAdmin";
        emit pwdLogin("admin", usernameAdmin, departmentAdmin);
    }
    else
    {
        for(const std::tuple<QString, QString, QPixmap, QString> &tmpUser : tmpUserList)
        {
            QString tmpUsername = std::get<0>(tmpUser);
            QString tmpUserPwd = std::get<1>(tmpUser);
            QString tmpUserDepartment = std::get<3>(tmpUser);
            if(username == tmpUsername && userPwd == tmpUserPwd)
            {
                qDebug()<<"username == tmpUsername && userPwd == tmpUserPwd";
                emit pwdLogin(username, username, tmpUserDepartment);
                return;
            }   
        }
    }
    
    QMessageBox::warning(NULL, "密码错误", "密码错误，请重试！");
}
