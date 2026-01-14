#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utf8.h"
#include "globalVariables.h"

#include <QMessageBox>
#include <QTimer>
#include <QBitmap>
#include <QPainter>
#include <QMouseEvent>
#include <QScreen>
#include <windows.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    iniUI();
    
    screenAdapter();
    
    iniWidget();
    
    iniViewModel();
    
    iniViewConnection();
    
    iniCurrentWidget();
    
    // 处理命令行参数
    QStringList arguments = QCoreApplication::arguments();
    if (arguments.contains("showAdminRegister"))
    {
        this->setVisible(false);
    }
    else if(arguments.contains("showTmpAuthorize"))
    {
        this->setVisible(false);
    }
    else
    {
        this->setVisible(false);
        if(adminLogin || firstLogin)
        {
            showCurrentWidget(cur);
        }
    }
    

//    // 处理命令行参数
//    QStringList arguments = QCoreApplication::arguments();
//    qDebug()<<arguments;
//    if (arguments.contains("showAdminRegister")) {
//        QTimer::singleShot(0, this, [this](){
//            pCheckPwdWidget->show();
//            this->hide();
//        });
//    }
//    else
//    {
//        this->show();
//    }
    
    server = new QLocalServer(this);
    if (!server->listen("ClientServer")) {
        QMessageBox::critical(this, "错误", "无法启动本地服务");
    }
    connect(server, &QLocalServer::newConnection, this, &MainWindow::handleNewConnection);
    
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::iniUI()
{
    //去掉边框
    setWindowFlag(Qt::FramelessWindowHint);
    //窗口半透明（因为背景图片采用外阴影，使用该选项才能去除窗口无色边框）
    setAttribute(Qt::WA_TranslucentBackground);

//    //遮罩使窗口变为圆角
//    QBitmap bmp(this->size());
//    bmp.fill();
//    QPainter p(&bmp);
//    p.setPen(Qt::NoPen);
//    p.setBrush(Qt::black);
//    p.drawRoundedRect(bmp.rect(), 10, 10);
//    setMask(bmp);
}

void MainWindow::screenAdapter()
{
    QScreen* pScreen = QGuiApplication::primaryScreen();
    QRect rect1 = pScreen->geometry();

    w_fx = (double)rect1.size().width() / 1920.0;
    h_fx = (double)rect1.size().height() / 1080.0;
    
    this->setMinimumSize(350 * w_fx, 180 * h_fx);
    this->setMaximumSize(350 * w_fx, 180 * h_fx);
    
    ui->widget->setMinimumSize(350 * w_fx, 180 * h_fx);
    ui->widget->setMaximumSize(350 * w_fx, 180 * h_fx);
    
    ui->frame_title->setMinimumSize(350 * w_fx, 40 * h_fx);
    ui->frame_title->setMaximumSize(350 * w_fx, 40 * h_fx);
    
    ui->horizontalLayout_3->setContentsMargins(0, 0, 9 * h_fx, 0);
    ui->horizontalLayout_3->setSpacing(6 * w_fx);
    
    ui->frame->setMinimumSize(213 * w_fx, 40 * h_fx);
    ui->frame->setMaximumSize(213 * w_fx, 40 * h_fx);
    
    ui->horizontalLayout->setContentsMargins(3 * w_fx, 0, 0, 0);
    ui->horizontalLayout->setSpacing(3 * w_fx);
    
    ui->label_logo->setMinimumSize(75 * w_fx, 25 * h_fx);
    ui->label_logo->setMaximumSize(75 * w_fx, 25 * h_fx);
    
    ui->label_2->setMinimumSize(130 * w_fx, 25 * h_fx);
    ui->label_2->setMaximumSize(130 * w_fx, 25 * h_fx);
    QFont font = ui->label_2->font();
    font.setPointSize(8 * w_fx);
    ui->label_2->setFont(font);
    
    ui->frame_2->setMinimumSize(55 * w_fx, 40 * h_fx);
    ui->frame_2->setMaximumSize(55 * w_fx, 40 * h_fx);
    
    ui->btn_min->setMinimumSize(20 * w_fx, 20 * w_fx);
    ui->btn_min->setMaximumSize(20 * w_fx, 20 * w_fx);
    
    ui->btn_close->setMinimumSize(20 * w_fx, 20 * w_fx);
    ui->btn_close->setMaximumSize(20 * w_fx, 20 * w_fx);
    
    ui->frame_content->setMinimumSize(350 * w_fx, 140 * h_fx);
    ui->frame_content->setMaximumSize(350 * w_fx, 140 * h_fx);
    
    ui->frame_3->setMinimumSize(350 * w_fx, 38 * h_fx);
    ui->frame_3->setMaximumSize(350 * w_fx, 38 * h_fx);
    
    ui->horizontalLayout_4->setContentsMargins(9 * w_fx, 9 * h_fx, 0, 9 * h_fx);
    ui->horizontalLayout_4->setSpacing(6 * w_fx);
    
    ui->label_3->setMinimumSize(20 * w_fx, 20 * h_fx);
    ui->label_3->setMaximumSize(20 * w_fx, 20 * h_fx);
    
    ui->label_4->setMinimumSize(60 * w_fx, 20 * h_fx);
    ui->label_4->setMaximumSize(60 * w_fx, 20 * h_fx);
    QFont font2 = ui->label_4->font();
    font2.setPointSize(8 * w_fx);
    ui->label_4->setFont(font2);
    
    ui->label->setMinimumSize(60 * w_fx, 20 * h_fx);
    ui->label->setMaximumSize(60 * w_fx, 20 * h_fx);
    QFont font3 = ui->label->font();
    font3.setPointSize(8 * w_fx);
    ui->label->setFont(font3);
    
    ui->frame_4->setMinimumSize(350 * w_fx, 100 * h_fx);
    ui->frame_4->setMaximumSize(350 * w_fx, 100 * h_fx);
    
    ui->horizontalLayout_5->setContentsMargins(9 * w_fx, 9 * h_fx, 9 * w_fx, 9 * h_fx);
    ui->horizontalLayout_5->setSpacing(6 * h_fx);
    
    ui->pushButton->setMinimumSize(136 * w_fx, 54 * h_fx);
    ui->pushButton->setMaximumSize(136 * w_fx, 54 * h_fx);
    
    ui->pushButton_2->setMinimumSize(136 * w_fx, 54 * h_fx);
    ui->pushButton_2->setMaximumSize(136 * w_fx, 54 * h_fx);
}

void MainWindow::iniWidget()
{
    pCheckPwdWidget = new checkPwdWidget();
    pCheckPwdWidget->setAttribute(Qt::WA_DeleteOnClose);
    
    pFirstLoginWidget = new firstLoginWidget();
    pFirstLoginWidget->setAttribute(Qt::WA_DeleteOnClose);
    
    pTmpAuthManageWidget = new tmpAuthManageWidget();
    pTmpAuthManageWidget->setAttribute(Qt::WA_DeleteOnClose);
    
    pTmpAuthWidget = new tmpAuthorizeWidget();
    pTmpAuthWidget->setAttribute(Qt::WA_DeleteOnClose);
    
    pFaceManageWidget = new faceManageWidget();
    pFaceManageWidget->setAttribute(Qt::WA_DeleteOnClose);
    
    pDetailWidget = new detailWidget();
    pDetailWidget->setAttribute(Qt::WA_DeleteOnClose);
    
    pTimeWidget = new timeWidget();
    pTimeWidget->setAttribute(Qt::WA_DeleteOnClose);
    
    pChooseFaceWidget = new chooseFaceWidget();
    pChooseFaceWidget->setAttribute(Qt::WA_DeleteOnClose);
    
    pFaceLoginWidget = new faceLoginWidget();
    pFaceLoginWidget->setAttribute(Qt::WA_DeleteOnClose);
    
    pPwdLoginWidget = new pwdLoginWidget();
    pPwdLoginWidget->setAttribute(Qt::WA_DeleteOnClose);
}

void MainWindow::iniViewConnection()
{
    connect(this, &MainWindow::reregister, this, &MainWindow::onReregister);
    connect(this, &MainWindow::showReturnBtn, pCheckPwdWidget, &checkPwdWidget::onShowReturnBtn);
    connect(this, &MainWindow::showReturnBtn, pFirstLoginWidget, &firstLoginWidget::onShowReturnBtn);
    
    connect(pFirstLoginWidget, &firstLoginWidget::success, this, &MainWindow::onReceiveSuccess);
    connect(pFirstLoginWidget, &firstLoginWidget::savePwd, pCheckPwdWidget, &checkPwdWidget::onReregisterCheckPwd);
    connect(pFirstLoginWidget, &firstLoginWidget::returnHomePage, this, &MainWindow::onReturnHomePage);
    connect(pFirstLoginWidget, &firstLoginWidget::returnFaceManagePage, this, &MainWindow::onReturnFaceManagePage);
    connect(pFirstLoginWidget, &firstLoginWidget::returnFaceManagePage2, this, &MainWindow::onReturnFaceManagePage2);
    connect(pFirstLoginWidget, &firstLoginWidget::returnDetailPage, this, &MainWindow::onReturnDetailPage);
    connect(pFirstLoginWidget, &firstLoginWidget::returnDetailPage2, this, &MainWindow::onReturnDetailPage2);
    
    connect(pCheckPwdWidget, &checkPwdWidget::checkSuccessful, this, &MainWindow::onCheckSuccessful);
    connect(pCheckPwdWidget, &checkPwdWidget::changeWidget, this, &MainWindow::onChangeWidget);
    connect(pCheckPwdWidget, &checkPwdWidget::reregisterCheckSuccessful, this, &MainWindow::onReceiveSuccess);
    connect(pCheckPwdWidget, &checkPwdWidget::returnHomePage, this, &MainWindow::onReturnHomePage);
    
    connect(pTmpAuthManageWidget, &tmpAuthManageWidget::returnHomePage, this, &MainWindow::onReturnHomePage);
    connect(pTmpAuthManageWidget, &tmpAuthManageWidget::authorize, this, &MainWindow::onAuthorize);
    connect(pTmpAuthManageWidget, &tmpAuthManageWidget::faceManage, this, &MainWindow::onFaceManage);  
    
    connect(pTmpAuthWidget, &tmpAuthorizeWidget::returnAuthManagePage, this, &MainWindow::onReturnAuthManagePage);
    connect(pTmpAuthWidget, &tmpAuthorizeWidget::chooseFace, this, &MainWindow::onChooseFace);
    
    connect(pFaceManageWidget, &faceManageWidget::returnTmpAuthManagePage, this, &MainWindow::onReturnTmpAuthManagePage);
    connect(pFaceManageWidget, &faceManageWidget::tmpUsrFaceRegister, this, &MainWindow::onTmpUsrFaceRegister);
    connect(pFaceManageWidget, &faceManageWidget::loadTmpUserDetail, this, &MainWindow::onLoadTmpUserDetail);
    
    connect(pDetailWidget, &detailWidget::returnFaceManagePage, this, &MainWindow::onReturnFaceManagePage2);
    connect(pDetailWidget, &detailWidget::changeFacialInfo, this, &MainWindow::onChangeFacialInfo);
    connect(pDetailWidget, &detailWidget::addTmpUserPeriod, this, &MainWindow::onAddTmpUserPeriod);
    connect(pDetailWidget, &detailWidget::changeTmpUserPeriod, this, &MainWindow::onChangeTmpUserPeriod);

    connect(pTimeWidget, &timeWidget::returnDetailPage, this, &MainWindow::onReturnDetailPage2);
    connect(pTimeWidget, &timeWidget::returnAuthManagePage, this, &MainWindow::onReturnAuthManagePage);
    connect(pTimeWidget, &timeWidget::addUserPeriodSuccess, this, &MainWindow::onAddUserPeriodSuccess);
    connect(pTimeWidget, &timeWidget::changeUserPeriodSuccess, this, &MainWindow::onChangeUserPeriodSuccess);
    connect(pTimeWidget, &timeWidget::changeUserPeriodSuccess2, this, &MainWindow::onChangeUserPeriodSuccess2);
    
    connect(pChooseFaceWidget, &chooseFaceWidget::sendTmpUserFaceInfo, this, &MainWindow::onSendTmpUserFaceInfo);
    connect(pChooseFaceWidget, &chooseFaceWidget::returnTmpAuthPage, this, &MainWindow::onReturnTmpAuthPage);

    connect(pFaceLoginWidget, &faceLoginWidget::checkPassed, this, &MainWindow::onCheckPassed);
    connect(pFaceLoginWidget, &faceLoginWidget::showPwdLoginPage, this, &MainWindow::onShowPwdLoginPage);

    connect(pPwdLoginWidget, &pwdLoginWidget::returnFaceLoginPage, this, &MainWindow::onReturnFaceLoginPage);
    connect(pPwdLoginWidget, &pwdLoginWidget::checkPassed, this, &MainWindow::onCheckPassed);
}

void MainWindow::iniViewModel()
{
    viewModel = new myViewModel();
    
    connect(this, &MainWindow::iniDatabase, viewModel, &myViewModel::onIniDatabase);
    connect(this, &MainWindow::checkUsername, viewModel, &myViewModel::onCheckUsername);
    connect(viewModel, &myViewModel::refreshUsername, this, &MainWindow::onRefeshUsername);
    connect(viewModel, &myViewModel::changeTmpUserPeriod2, this, &MainWindow::onChangeTmpUserPeriod2);
    
    connect(pCheckPwdWidget, &checkPwdWidget::checkUserAndPwd, viewModel, &myViewModel::onCheckUserAndPwd);
    connect(pCheckPwdWidget, &checkPwdWidget::checkUserAndPwdOnly, viewModel, &myViewModel::onCheckUserAndPwdOnly);
    connect(pCheckPwdWidget, &checkPwdWidget::saveUserAndPwd, viewModel, &myViewModel::onSaveUserAndPwd);
    connect(viewModel, &myViewModel::sendCheckResult, pCheckPwdWidget, &checkPwdWidget::onSendCheckResult);
    connect(viewModel, &myViewModel::sendCheckOnlyResult, pCheckPwdWidget, &checkPwdWidget::onSendCheckOnlyResult);  
    
    connect(pFirstLoginWidget, &firstLoginWidget::iniCam, viewModel, &myViewModel::onIniCam);
    connect(pFirstLoginWidget, &firstLoginWidget::openCam, viewModel, &myViewModel::onOpenCam);
    connect(pFirstLoginWidget, &firstLoginWidget::uniniCam, viewModel, &myViewModel::onUniniCam);
    connect(pFirstLoginWidget, &firstLoginWidget::startDetection, viewModel, &myViewModel::onStartDetection);
    connect(pFirstLoginWidget, &firstLoginWidget::userRegistration, viewModel, &myViewModel::onUserRegistration);
    connect(pFirstLoginWidget, &firstLoginWidget::userReregistration, viewModel, &myViewModel::onUserReregistration);
    connect(pFirstLoginWidget, &firstLoginWidget::tmpUserRegistration, viewModel, &myViewModel::onTmpUserRegistration2);
    connect(pFirstLoginWidget, &firstLoginWidget::changeTmpUserInfo, viewModel, &myViewModel::onChangeTmpUserInfo);
    connect(pFirstLoginWidget, &firstLoginWidget::delTmpUserEncFile, viewModel, &myViewModel::onDelTmpUserEncFile);
    connect(viewModel, &myViewModel::repostCamMessage, pFirstLoginWidget, &firstLoginWidget::onRepostCamMessage);
    connect(viewModel, &myViewModel::sendPixmap, pFirstLoginWidget, &firstLoginWidget::onSendPixmap);
    connect(viewModel, &myViewModel::sendVmMessage, pFirstLoginWidget, &firstLoginWidget::onSendVmMessage);
    connect(viewModel, &myViewModel::sendRegisterResult, pFirstLoginWidget, &firstLoginWidget::onSendRegisterResult);    
    connect(viewModel, &myViewModel::sendTmpRegisterResult2, pFirstLoginWidget, &firstLoginWidget::onSendTmpRegisterResult2);
    connect(viewModel, &myViewModel::sendUpdateTmpUsrInfoRes, pFirstLoginWidget, &firstLoginWidget::onSendUpdateTmpUsrInfoRes);
    
    connect(pTmpAuthManageWidget, &tmpAuthManageWidget::getAllTmpUserData, viewModel, &myViewModel::onGetAllTmpUserData);
    connect(pTmpAuthManageWidget, &tmpAuthManageWidget::getTempUserLogin, viewModel, &myViewModel::onGetTempUserLogin);    
    connect(pTmpAuthManageWidget, &tmpAuthManageWidget::setTempUserLogin, viewModel, &myViewModel::onSetTempUserLogin);        
    connect(pTmpAuthManageWidget, &tmpAuthManageWidget::deleteTmpUserData, viewModel, &myViewModel::onDeleteTmpUserData);    
    connect(pTmpAuthManageWidget, &tmpAuthManageWidget::changeTmpUserPeriod, viewModel, &myViewModel::onChangeTmpUserPeriod);    
    connect(viewModel, &myViewModel::sendAllTmpUserData, pTmpAuthManageWidget, &tmpAuthManageWidget::onSendAllTmpUserData);    
    connect(viewModel, &myViewModel::sendTmpUserLogin, pTmpAuthManageWidget, &tmpAuthManageWidget::onSendTmpUserLogin);      
    connect(viewModel, &myViewModel::sendDelTmpUserDataResult, pTmpAuthManageWidget, &tmpAuthManageWidget::onSendDelTmpUserDataResult);
    
    connect(pTmpAuthWidget, &tmpAuthorizeWidget::iniCam, viewModel, &myViewModel::onIniCam);
    connect(pTmpAuthWidget, &tmpAuthorizeWidget::uniniCam, viewModel, &myViewModel::onUniniCam);
    connect(pTmpAuthWidget, &tmpAuthorizeWidget::openCam, viewModel, &myViewModel::onOpenCam);
    connect(pTmpAuthWidget, &tmpAuthorizeWidget::startDetection, viewModel, &myViewModel::onStartDetection);
    connect(pTmpAuthWidget, &tmpAuthorizeWidget::tmpUserRegistration, viewModel, &myViewModel::onTmpUserRegistration);
    connect(viewModel, &myViewModel::repostCamMessage, pTmpAuthWidget, &tmpAuthorizeWidget::onRepostCamMessage);
    connect(viewModel, &myViewModel::sendPixmap, pTmpAuthWidget, &tmpAuthorizeWidget::onSendPixmap);
    connect(viewModel, &myViewModel::sendVmMessage, pTmpAuthWidget, &tmpAuthorizeWidget::onSendVmMessage);
    connect(viewModel, &myViewModel::sendTmpRegisterResult, pTmpAuthWidget, &tmpAuthorizeWidget::onSendTmpRegisterResult);
        
    connect(pFaceManageWidget, &faceManageWidget::getAllTmpUsrPicInfo, viewModel, &myViewModel::onGetAllTmpUsrPicInfo);
    connect(viewModel, &myViewModel::sendTmpUsrInfo, pFaceManageWidget, &faceManageWidget::onSendTmpUsrInfo);
    connect(pFaceManageWidget, &faceManageWidget::getTmpUsrPicInfo, viewModel, &myViewModel::onGetTmpUsrPicInfo);
    
    connect(pDetailWidget, &detailWidget::getTmpUserData, viewModel, &myViewModel::onGetTmpUserData);
    connect(pDetailWidget, &detailWidget::deleteTmpUser, viewModel, &myViewModel::onDeleteTmpUser);
    connect(pDetailWidget, &detailWidget::getFacePixmap, viewModel, &myViewModel::onGetFacePixmap);
    connect(pDetailWidget, &detailWidget::delTmpUserEncFile, viewModel, &myViewModel::onDelTmpUserEncFile);
    connect(pDetailWidget, &detailWidget::deleteTmpUserData, viewModel, &myViewModel::onDeleteTmpUserData2);
    connect(viewModel, &myViewModel::sendTmpUserPeriodsData, pDetailWidget, &detailWidget::onSendTmpUserPeriodsData);
    connect(viewModel, &myViewModel::sendDeleteTmpUserRes, pDetailWidget, &detailWidget::onSendDeleteTmpUserRes);
    connect(viewModel, &myViewModel::sendFacePixmap, pDetailWidget, &detailWidget::onSendFacePixmap);
    connect(viewModel, &myViewModel::sendDelTmpUserDataResult2, pDetailWidget, &detailWidget::onSendDelTmpUserDataResult2);
    
    connect(pTimeWidget, &timeWidget::addUserPeriod, viewModel, &myViewModel::onAddUserPeriod);
    connect(pTimeWidget, &timeWidget::changeUserPeriod, viewModel, &myViewModel::onChangeUserPeriod);
    connect(viewModel, &myViewModel::sendAddUserPeriodRes, pTimeWidget, &timeWidget::onSendAddUserPeriodRes);
    connect(viewModel, &myViewModel::sendChangeUserPeriodRes, pTimeWidget, &timeWidget::onSendChangeUserPeriodRes);
    
    connect(pChooseFaceWidget, &chooseFaceWidget::getAllTmpUsrFaceInfo, viewModel, &myViewModel::onGetAllTmpUsrFaceInfo);
    connect(viewModel, &myViewModel::sendTmpUsrFaceInfo, pChooseFaceWidget, &chooseFaceWidget::onSendTmpUsrFaceInfo);
    connect(pChooseFaceWidget, &chooseFaceWidget::getTmpUsrFaceInfo, viewModel, &myViewModel::onGetTmpUsrFaceInfo);
       
    connect(pFaceLoginWidget, &faceLoginWidget::iniCam, viewModel, &myViewModel::onIniCam);
    connect(pFaceLoginWidget, &faceLoginWidget::openCam, viewModel, &myViewModel::onOpenCam);
    connect(pFaceLoginWidget, &faceLoginWidget::startDetection, viewModel, &myViewModel::onStartDetection);  
    connect(pFaceLoginWidget, &faceLoginWidget::checkUserFace, viewModel, &myViewModel::onCheckUserFace);
    connect(pFaceLoginWidget, &faceLoginWidget::uniniCam, viewModel, &myViewModel::onUniniCam);
    connect(viewModel, &myViewModel::repostCamMessage, pFaceLoginWidget, &faceLoginWidget::onRepostCamMessage);
    connect(viewModel, &myViewModel::sendVmMessage, pFaceLoginWidget, &faceLoginWidget::onSendVmMessage);
    connect(viewModel, &myViewModel::sendPixmap, pFaceLoginWidget, &faceLoginWidget::onSendPixmap);
    connect(viewModel, &myViewModel::sendUserIdentity, pFaceLoginWidget, &faceLoginWidget::onSendUserIdentity);
    
    connect(pPwdLoginWidget, &pwdLoginWidget::checkUserPwd, viewModel, &myViewModel::onCheckUserPwd);
    connect(viewModel, &myViewModel::sendPwdCheckResult, pPwdLoginWidget, &pwdLoginWidget::onSendPwdCheckResult);
    
    connect(this, &MainWindow::iniCheckThread, viewModel, &myViewModel::onIniCheckThread);
    connect(this, &MainWindow::iniSyncThread, viewModel, &myViewModel::onIniSyncThread);
    connect(pFirstLoginWidget, &firstLoginWidget::success, viewModel, &myViewModel::startSyncWork);
    connect(pFirstLoginWidget, &firstLoginWidget::savePwd, viewModel, &myViewModel::startSyncWork);
    connect(pFirstLoginWidget, &firstLoginWidget::startSync, viewModel, &myViewModel::startSyncWork);
    connect(pTmpAuthWidget, &tmpAuthorizeWidget::startSync, viewModel, &myViewModel::startSyncWork);
    connect(pDetailWidget, &detailWidget::startSync, viewModel, &myViewModel::startSyncWork);
    connect(pTmpAuthManageWidget, &tmpAuthManageWidget::startSync, viewModel, &myViewModel::startSyncWork);
    connect(pTimeWidget, &timeWidget::startSync, viewModel, &myViewModel::startSyncWork);
    
    emit iniCheckThread();
    emit iniDatabase();
    emit iniSyncThread();
}

void MainWindow::iniCurrentWidget()
{
    QString lastLoginIdentity = viewModel->getLastLoginIdentity();
    adminLogin = lastLoginIdentity == "admin" ? true : false;
    firstLogin = viewModel->isFirstLogin();
    bool checkPwd = viewModel->isPwdChecked();
    
    if(!checkPwd)
    {
        cur = checkpwdwidget;
    }
    else if(checkPwd && firstLogin)
    {
        pFirstLoginWidget->type = admin;
        cur = firstloginwidget;
    }
    else
    {
        if(lastLoginIdentity != "admin")
        {
            showCurrentWidget(faceloginwidget);
        }
        cur = homepage;
    }
//    showCurrentWidget(cur);
}

void MainWindow::handleNewConnection()
{
    clientSocket = server->nextPendingConnection();
    connect(clientSocket, &QLocalSocket::readyRead, this, &MainWindow::readCommand);
}

void MainWindow::readCommand()
{
    QString command = QString::fromUtf8(clientSocket->readAll());
    if (command == "showAdminRegister")
    {        
        if(cur == homepage)
        {
            emit reregister();  
        }
        else
        {
            QMessageBox::information(0, "提示", "请先完成当前操作");
            showCurrentWidget(cur);
        }
    } 
    else if (command == "showTmpAuthorize")
    {
        if(cur == homepage)
        {
            cur = tmpauthmanagewidget;
            showCurrentWidget(cur);
        }
        else
        {
            QMessageBox::information(0, "提示", "请先完成当前操作");
            showCurrentWidget(cur);
        }
    }
    else if (command == "shutdown")
    {
        QCoreApplication::quit();
    } 
    else if (command == "showCurrent")
    {
        showCurrentWidget(cur);
    }
    clientSocket->disconnectFromServer();
}

void MainWindow::showCurrentWidget(currentWidget cur)
{
    switch(cur)
    {
    case homepage:   
        this->showMinimized();
        this->showNormal();  
        pFirstLoginWidget->setVisible(false);
        pCheckPwdWidget->setVisible(false);
        pTmpAuthManageWidget->setVisible(false);
        pTmpAuthWidget->setVisible(false);
        pFaceManageWidget->setVisible(false);
        pDetailWidget->setVisible(false);
        pTimeWidget->setVisible(false);
        pChooseFaceWidget->setVisible(false);
        pFaceLoginWidget->setVisible(false);
        pPwdLoginWidget->setVisible(false);
        emit checkUsername();
        break;
        
    case checkpwdwidget:
        this->hide();
        pFirstLoginWidget->setVisible(false);
        pTmpAuthManageWidget->setVisible(false);
        pTmpAuthWidget->setVisible(false);
        pFaceManageWidget->setVisible(false);
        pDetailWidget->setVisible(false);
        pTimeWidget->setVisible(false);
        pChooseFaceWidget->setVisible(false);
        pFaceLoginWidget->setVisible(false);
        pPwdLoginWidget->setVisible(false);
        pCheckPwdWidget->showMinimized();
        pCheckPwdWidget->showNormal();
        break;
        
    case firstloginwidget:
        pFirstLoginWidget->iniCamera(); 
        this->hide();
        pCheckPwdWidget->setVisible(false);
        pTmpAuthManageWidget->setVisible(false);
        pTmpAuthWidget->setVisible(false);
        pFaceManageWidget->setVisible(false);
        pDetailWidget->setVisible(false);
        pTimeWidget->setVisible(false);
        pChooseFaceWidget->setVisible(false);
        pFaceLoginWidget->setVisible(false);
        pPwdLoginWidget->setVisible(false);   
        pFirstLoginWidget->showMinimized();
        pFirstLoginWidget->showNormal();
        break;
        
    case tmpauthmanagewidget:
        this->hide();
        pCheckPwdWidget->setVisible(false);
        pFirstLoginWidget->setVisible(false);
        pTmpAuthWidget->setVisible(false);
        pFaceManageWidget->setVisible(false);
        pDetailWidget->setVisible(false);
        pTimeWidget->setVisible(false);
        pChooseFaceWidget->setVisible(false);
        pFaceLoginWidget->setVisible(false);
        pPwdLoginWidget->setVisible(false);
        emit pTmpAuthManageWidget->getAllTmpUserData();
        pTmpAuthManageWidget->iniCheckBox();
        pTmpAuthManageWidget->showMinimized();
        pTmpAuthManageWidget->showNormal();
        break;
        
    case tmpauthwidget:
        this->hide();
        pTmpAuthWidget->iniCamera();
        pCheckPwdWidget->setVisible(false);
        pFirstLoginWidget->setVisible(false);
        pTmpAuthManageWidget->setVisible(false);
        pFaceManageWidget->setVisible(false);
        pDetailWidget->setVisible(false);
        pTimeWidget->setVisible(false);
        pChooseFaceWidget->setVisible(false);
        pFaceLoginWidget->setVisible(false);
        pPwdLoginWidget->setVisible(false);
        pTmpAuthWidget->showMinimized();
        pTmpAuthWidget->showNormal();
        pTmpAuthWidget->iniUI();
        break;
        
    case facemanagewidget:
        this->hide();
        pCheckPwdWidget->setVisible(false);
        pFirstLoginWidget->setVisible(false);
        pTmpAuthManageWidget->setVisible(false);
        pTmpAuthWidget->setVisible(false);
        pDetailWidget->setVisible(false);
        pTimeWidget->setVisible(false);
        pChooseFaceWidget->setVisible(false);
        pFaceLoginWidget->setVisible(false);
        pPwdLoginWidget->setVisible(false);
        pFaceManageWidget->getAllTmpUsrPicInfo();
        pFaceManageWidget->showMinimized();
        pFaceManageWidget->showNormal();
        break;
        
    case facemanagewidget2:
        this->hide();
        pCheckPwdWidget->setVisible(false);
        pFirstLoginWidget->setVisible(false);
        pTmpAuthManageWidget->setVisible(false);
        pTmpAuthWidget->setVisible(false);
        pDetailWidget->setVisible(false);
        pTimeWidget->setVisible(false);
        pChooseFaceWidget->setVisible(false);
        pFaceLoginWidget->setVisible(false);
        pPwdLoginWidget->setVisible(false);
        pFaceManageWidget->showMinimized();
        pFaceManageWidget->showNormal();
        break;
        
    case detailwidget:
        this->hide();
        pCheckPwdWidget->setVisible(false);
        pFirstLoginWidget->setVisible(false);
        pTmpAuthManageWidget->setVisible(false);
        pTmpAuthWidget->setVisible(false);
        pFaceManageWidget->setVisible(false);
        pTimeWidget->setVisible(false);
        pChooseFaceWidget->setVisible(false);
        pFaceLoginWidget->setVisible(false);
        pPwdLoginWidget->setVisible(false);
        pDetailWidget->showMinimized();
        pDetailWidget->showNormal();
        break;
        
    case detailwidget2:
        this->hide();
        pCheckPwdWidget->setVisible(false);
        pFirstLoginWidget->setVisible(false);
        pTmpAuthManageWidget->setVisible(false);
        pTmpAuthWidget->setVisible(false);
        pFaceManageWidget->setVisible(false);
        pTimeWidget->setVisible(false);
        pChooseFaceWidget->setVisible(false);
        pFaceLoginWidget->setVisible(false);
        pPwdLoginWidget->setVisible(false);
        pDetailWidget->reloadUserInfo();
        pDetailWidget->showMinimized();
        pDetailWidget->showNormal();
        break;
        
    case detailwidget3:
        this->hide();
        pCheckPwdWidget->setVisible(false);
        pFirstLoginWidget->setVisible(false);
        pTmpAuthManageWidget->setVisible(false);
        pTmpAuthWidget->setVisible(false);
        pFaceManageWidget->setVisible(false);
        pTimeWidget->setVisible(false);
        pChooseFaceWidget->setVisible(false);
        pFaceLoginWidget->setVisible(false);
        pPwdLoginWidget->setVisible(false);
        pDetailWidget->reloadUserInfo();
        pDetailWidget->reloadUserPeriod();
        pDetailWidget->showMinimized();
        pDetailWidget->showNormal();
        break;
        
    case timewidget:
        this->hide();
        pCheckPwdWidget->setVisible(false);
        pFirstLoginWidget->setVisible(false);
        pTmpAuthManageWidget->setVisible(false);
        pTmpAuthWidget->setVisible(false);
        pFaceManageWidget->setVisible(false);
        pDetailWidget->setVisible(false);
        pChooseFaceWidget->setVisible(false);
        pFaceLoginWidget->setVisible(false);
        pPwdLoginWidget->setVisible(false);
        pTimeWidget->showMinimized();
        pTimeWidget->showNormal();
        break;
        
    case choosefacewidget:
        this->hide();
        pCheckPwdWidget->setVisible(false);
        pFirstLoginWidget->setVisible(false);
        pTmpAuthManageWidget->setVisible(false);
        pTmpAuthWidget->setVisible(false);
        pFaceManageWidget->setVisible(false);
        pDetailWidget->setVisible(false);
        pTimeWidget->setVisible(false);
        pFaceLoginWidget->setVisible(false);
        pPwdLoginWidget->setVisible(false);
        pChooseFaceWidget->getAllTmpUsrFaceInfo();
        pChooseFaceWidget->showMinimized();
        pChooseFaceWidget->showNormal();
        break;
    
    case faceloginwidget:
        pFaceLoginWidget->iniCamera();
        this->hide();
        pCheckPwdWidget->setVisible(false);
        pFirstLoginWidget->setVisible(false);
        pTmpAuthManageWidget->setVisible(false);
        pTmpAuthWidget->setVisible(false);
        pFaceManageWidget->setVisible(false);
        pDetailWidget->setVisible(false);
        pTimeWidget->setVisible(false);
        pChooseFaceWidget->setVisible(false);
        pPwdLoginWidget->setVisible(false);
        pFaceLoginWidget->showMinimized();
        pFaceLoginWidget->showNormal();
        break;
        
    case pwdloginwidget:
        this->hide();
        pCheckPwdWidget->setVisible(false);
        pFirstLoginWidget->setVisible(false);
        pTmpAuthManageWidget->setVisible(false);
        pTmpAuthWidget->setVisible(false);
        pFaceManageWidget->setVisible(false);
        pDetailWidget->setVisible(false);
        pTimeWidget->setVisible(false);
        pChooseFaceWidget->setVisible(false);
        pFaceLoginWidget->setVisible(false);
        pPwdLoginWidget->showMinimized();
        pPwdLoginWidget->showNormal();
        break;
        
    default:
        break;
    }
}

void MainWindow::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        whereismouse = event->pos();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() == Qt::LeftButton)
    {
        //当窗口最大化或最小化时也不进行触发
        if (MainWindow::isFullScreen() || MainWindow::isMaximized() || MainWindow::isMinimized())
        {
            return;
        }
        else
        {
            //当在按钮之类需要鼠标操作的地方不进行触发(防误触)
            if (ui->btn_close->underMouse() || ui->btn_min->underMouse()
                    || ui->frame_content->underMouse())
            {
            }
            else
            {
                MainWindow::move(MainWindow::mapToGlobal(event->pos() - whereismouse));//移动
            }
        }
    }
    event->accept();
}

void MainWindow::onRefeshUsername(QString username)
{
    ui->label->setText(username);
}

void MainWindow::onCheckSuccessful()
{
    cur = firstloginwidget;
    pFirstLoginWidget->type = admin;
    showCurrentWidget(cur);
}

void MainWindow::onReceiveSuccess()
{
    cur = homepage;
    showCurrentWidget(cur);
    emit checkUsername();
    
//    cur = pwdcofigwidget;
//    showCurrentWidget(cur);
}

void MainWindow::onReregister()
{
    cur = checkpwdwidget;
    emit showReturnBtn();
    showCurrentWidget(cur);
}

void MainWindow::onChangeWidget()
{
    cur = firstloginwidget;
    pFirstLoginWidget->type = admin;
    showCurrentWidget(cur);
}

void MainWindow::onReturnHomePage()
{
    cur = homepage;
    showCurrentWidget(cur);
}

void MainWindow::onReturnFaceManagePage()
{
    cur = facemanagewidget2;
    showCurrentWidget(cur);
}

void MainWindow::onReturnFaceManagePage2()
{
    cur = facemanagewidget;
    showCurrentWidget(cur);
}

void MainWindow::onReturnAuthManagePage()
{
    cur = tmpauthmanagewidget;
    showCurrentWidget(cur);
}

void MainWindow::onAuthorize()
{
    cur = tmpauthwidget;
    showCurrentWidget(cur);
}

void MainWindow::onFaceManage()
{
    cur = facemanagewidget;
    showCurrentWidget(cur);
}

void MainWindow::onReturnTmpAuthManagePage()
{
    cur = tmpauthmanagewidget;
    showCurrentWidget(cur);
}

void MainWindow::onTmpUsrFaceRegister()
{
    cur = firstloginwidget;
    pFirstLoginWidget->type = tmpUser;
    showReturnBtn();
    showCurrentWidget(cur);
}

void MainWindow::onLoadTmpUserDetail(const QString &uuidStr, const QString &username, QPixmap pixmap)
{
    pDetailWidget->loadTmpUserDetailInfo(uuidStr, username, pixmap);
    cur = detailwidget;
    showCurrentWidget(cur);
}

void MainWindow::onChangeFacialInfo(const QString& tmpUserUuid, const QString& tmpUsername)
{
    cur = firstloginwidget;
    pFirstLoginWidget->type = changeFace;
    pFirstLoginWidget->curUuid = tmpUserUuid;
    pFirstLoginWidget->curUsername = tmpUsername;
    
    showReturnBtn();
    showCurrentWidget(cur);
    pFirstLoginWidget->setUsername(tmpUsername);
}

void MainWindow::onAddTmpUserPeriod(const QString &tmpUserUuid, const QString &tmpUsername, QPixmap tmpPixmap)
{
    cur = timewidget;
    pTimeWidget->curType = add;
    
    pTimeWidget->curUuid = tmpUserUuid;
    pTimeWidget->curUsername = tmpUsername;
    showCurrentWidget(cur);
    pTimeWidget->setUsername(tmpUsername);
    pTimeWidget->setPixmap(tmpPixmap);
    pTimeWidget->setDateTime();
}

void MainWindow::onChangeTmpUserPeriod(const QString &uuidStr, const QString &tmpUsername, QPixmap tmpPixmap, const QString &startTime, const QString &endTime)
{
    cur = timewidget;
    pTimeWidget->curType = change1;
    
    pTimeWidget->curUuid = uuidStr;
    pTimeWidget->curUsername = tmpUsername;
    showCurrentWidget(cur);
    pTimeWidget->setUsername(tmpUsername);
    pTimeWidget->setPixmap(tmpPixmap);
    pTimeWidget->setDateTime(startTime, endTime);
}

void MainWindow::onChangeTmpUserPeriod2(const QString &uuidStr, const QString &tmpUsername, QPixmap tmpPixmap, const QString &startTime, const QString &endTime)
{
    cur = timewidget;
    pTimeWidget->curType = change2;
    
    pTimeWidget->curUuid = uuidStr;
    pTimeWidget->curUsername = tmpUsername;
    showCurrentWidget(cur);
    pTimeWidget->setUsername(tmpUsername);
    pTimeWidget->setPixmap(tmpPixmap);
    pTimeWidget->setDateTime(startTime, endTime);
}

void MainWindow::onReturnDetailPage()
{
    cur = detailwidget2;
    showCurrentWidget(cur);
}

void MainWindow::onReturnDetailPage2()
{
    cur = detailwidget;
    showCurrentWidget(cur);
}

void MainWindow::onAddUserPeriodSuccess()
{
    cur = detailwidget3;
    showCurrentWidget(cur);
}

void MainWindow::onChangeUserPeriodSuccess()
{
    cur = detailwidget3;
    showCurrentWidget(cur);
}

void MainWindow::onChangeUserPeriodSuccess2()
{
    cur = tmpauthmanagewidget;
    showCurrentWidget(cur);
}

void MainWindow::onSendTmpUserFaceInfo(const QString &uuidStr, const QString &username, QPixmap pixmap)
{ 
    cur = tmpauthwidget;
    showCurrentWidget(cur);
    pTmpAuthWidget->setPixmap(pixmap);
    pTmpAuthWidget->setUsername(username);
}

void MainWindow::onReturnTmpAuthPage()
{
    cur = tmpauthwidget;
    showCurrentWidget(cur);
}

void MainWindow::onChooseFace()
{
    cur = choosefacewidget;
    showCurrentWidget(cur);
}

void MainWindow::onSetConfigPwdSuccess()
{
    cur = homepage;
    showCurrentWidget(cur);
    emit checkUsername();
}

void MainWindow::onCheckPassed()
{
    adminLogin = true;
    showCurrentWidget(cur);
}

void MainWindow::onShowPwdLoginPage()
{
    showCurrentWidget(pwdloginwidget);
}

void MainWindow::onReturnFaceLoginPage()
{
    showCurrentWidget(faceloginwidget);
}

void MainWindow::on_pushButton_clicked()
{
    emit reregister();
}

void MainWindow::on_pushButton_2_clicked()
{
    cur = tmpauthmanagewidget;
    showCurrentWidget(cur);
}

void MainWindow::on_btn_min_clicked()
{
    MainWindow::showMinimized();
}

void MainWindow::on_btn_close_clicked()
{
    MainWindow::close();
}
