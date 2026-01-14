#include "firstloginwidget.h"
#include "ui_firstloginwidget.h"
#include "utf8.h"
#include "globalVariables.h"
#include "floatingerrorwindow.h"

#include <QAction>
#include <QScreen>
#include <QDebug>
#include <QKeyEvent>
#include <QBitmap>
#include <QPainter>

firstLoginWidget::firstLoginWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::firstLoginWidget)
{
    ui->setupUi(this);

    iniUI();
    screenAdapt();
}

firstLoginWidget::~firstLoginWidget()
{
    delete ui;
}

void firstLoginWidget::iniUI()
{
    //去掉边框
    setWindowFlag(Qt::FramelessWindowHint);
    
    QAction *action = new QAction(this);
    action->setIcon(QIcon(":/images/images/user.png"));
    ui->lineEdit_user->addAction(action,QLineEdit::LeadingPosition);
    ui->lineEdit_user->setPlaceholderText("请输入用户名");
    
    QAction *action2 = new QAction(this);
    action2->setIcon(QIcon(":/images/images/password.png"));
    ui->lineEdit_pwd->addAction(action2,QLineEdit::LeadingPosition);
    ui->lineEdit_pwd->setPlaceholderText("请输入密码");
    
    ui->lineEdit_status->setPlaceholderText("相机未初始化");

    ui->stackedWidget->setCurrentIndex(0);

    ui->btn_return->setVisible(false);
    ui->btn_return->setCursor(QCursor(Qt::PointingHandCursor));   
}

void firstLoginWidget::screenAdapt()
{
    if(w_fx == 0 || h_fx ==0)
    {
        QScreen* pScreen = QGuiApplication::primaryScreen();
        QRect rect1 = pScreen->geometry();

        w_fx = (double)rect1.size().width() / 1920.0;
        h_fx = (double)rect1.size().height() / 1080.0;
    }

    //遮罩使窗口变为圆角
    QBitmap bmp(QSize(550 * w_fx, 400 * h_fx));
    bmp.fill();
    QPainter p(&bmp);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::black);
    p.drawRoundedRect(bmp.rect(), 10, 10);
    setMask(bmp);
    
//    this->setFixedSize(928 * w_fx, 556 * h_fx);
//    this->setFixedSize(960 * w_fx, 582 * h_fx);
    this->setMinimumSize(550 * w_fx, 400 * h_fx);
    this->setMaximumSize(550 * w_fx, 400 * h_fx);
    
    ui->widget->setMinimumSize(550 * w_fx, 400 * h_fx);
    ui->widget->setMaximumSize(550 * w_fx, 400 * h_fx);
    
    ui->gridLayout_6->setVerticalSpacing(2 * h_fx);
    
    ui->frame_title->setMinimumSize(550 * w_fx, 40 * h_fx);
    ui->frame_title->setMaximumSize(550 * w_fx, 40 * h_fx);
    
    ui->horizontalLayout_title->setContentsMargins(0, 0, 9 * w_fx, 0);
    ui->horizontalLayout_title->setSpacing(6 * w_fx);
    
    ui->frame_left->setMinimumSize(213 * w_fx, 40 * h_fx);
    ui->frame_left->setMaximumSize(213 * w_fx, 40 * h_fx);
    
    ui->horizontalLayout_left->setContentsMargins(3 * w_fx, 0, 0, 0);
    ui->horizontalLayout_left->setSpacing(3 * w_fx);
    
    ui->label_logo->setMinimumSize(75 * w_fx, 25 * h_fx);
    ui->label_logo->setMaximumSize(75 * w_fx, 25 * h_fx);
    
    ui->label_title->setMinimumSize(130 * w_fx, 25 * h_fx);
    ui->label_title->setMaximumSize(130 * w_fx, 25 * h_fx);
    QFont font = ui->label_title->font();
    font.setPointSize(8 * w_fx);
    ui->label_title->setFont(font);
    
    ui->frame_right->setMinimumSize(80 * w_fx, 40 * h_fx);
    ui->frame_right->setMaximumSize(80 * w_fx, 40 * h_fx);
    
    ui->frame_return->setMinimumSize(20 * w_fx, 20 * w_fx);
    ui->frame_return->setMaximumSize(20 * w_fx, 20 * w_fx);
    
    ui->btn_return->setMinimumSize(20 * w_fx, 20 * w_fx);
    ui->btn_return->setMaximumSize(20 * w_fx, 20 * w_fx);
    
    ui->btn_min->setMinimumSize(20 * w_fx, 20 * w_fx);
    ui->btn_min->setMaximumSize(20 * w_fx, 20 * w_fx);
    
    ui->btn_close->setMinimumSize(20 * w_fx, 20 * w_fx);
    ui->btn_close->setMaximumSize(20 * w_fx, 20 * w_fx);
    
    ui->frame->setMinimumSize(550 * w_fx, 94 * h_fx);
    ui->frame->setMaximumSize(550 * w_fx, 94 * h_fx);

    ui->gridLayout_4->setVerticalSpacing(2 * h_fx);
    
    ui->label->setMinimumSize(70 * w_fx, 22 * h_fx);
    ui->label->setMaximumSize(70 * w_fx, 22 * h_fx);
    font = ui->label->font();
    font.setPointSize(9 * w_fx);
    ui->label->setFont(font);
    
    ui->label_2->setMinimumSize(70 * w_fx, 22 * h_fx);
    ui->label_2->setMaximumSize(70 * w_fx, 22 * h_fx);
    font = ui->label_2->font();
    font.setPointSize(9 * w_fx);
    ui->label_2->setFont(font);
    
    ui->label_4->setMinimumSize(70 * w_fx, 22 * h_fx);
    ui->label_4->setMaximumSize(70 * w_fx, 22 * h_fx);
    font = ui->label_4->font();
    font.setPointSize(9 * w_fx);
    ui->label_4->setFont(font);
    
    ui->label_5->setMinimumSize(70 * w_fx, 22 * h_fx);
    ui->label_5->setMaximumSize(70 * w_fx, 22 * h_fx);
    font = ui->label_5->font();
    font.setPointSize(9 * w_fx);
    ui->label_5->setFont(font);
    
    ui->lineEdit_user->setMinimumSize(375 * w_fx, 22 * h_fx);
    ui->lineEdit_user->setMaximumSize(375 * w_fx, 22 * h_fx);
    font = ui->lineEdit_user->font();
    font.setPointSize(9 * w_fx);
    ui->lineEdit_user->setFont(font);
    
    ui->lineEdit_pwd->setMinimumSize(375 * w_fx, 22 * h_fx);
    ui->lineEdit_pwd->setMaximumSize(375 * w_fx, 22 * h_fx);
    font = ui->lineEdit_pwd->font();
    font.setPointSize(9 * w_fx);
    ui->lineEdit_pwd->setFont(font);
    
    ui->lineEdit_status->setMinimumSize(346 * w_fx, 22 * h_fx);
    ui->lineEdit_status->setMaximumSize(346 * w_fx, 22 * h_fx);
    font = ui->lineEdit_status->font();
    font.setPointSize(9 * w_fx);
    ui->lineEdit_status->setFont(font);
    
    ui->lineEdit_department->setMinimumSize(375 * w_fx, 22 * h_fx);
    ui->lineEdit_department->setMaximumSize(375 * w_fx, 22 * h_fx);
    font = ui->lineEdit_department->font();
    font.setPointSize(9 * w_fx);
    ui->lineEdit_department->setFont(font);
    
    ui->btn_refresh->setMinimumSize(20 * w_fx, 20 * w_fx);
    ui->btn_refresh->setMaximumSize(20 * w_fx, 20 * w_fx);
    
    ui->stackedWidget->setMinimumSize(310 * w_fx, 234 * h_fx);
    ui->stackedWidget->setMaximumSize(310 * w_fx, 234 * h_fx);
    
    ui->page1->setMinimumSize(310 * w_fx, 234 * h_fx);
    ui->page1->setMaximumSize(310 * w_fx, 234 * h_fx);
    
    ui->gridLayout_2->setContentsMargins(9 * w_fx, 5 * h_fx, 9 * w_fx, 5 * h_fx);
    ui->gridLayout_2->setHorizontalSpacing(6 * w_fx);
    ui->gridLayout_2->setVerticalSpacing(0 * h_fx);
    
    ui->btn_img->setMinimumSize(200 * w_fx, 200 * w_fx);
    ui->btn_img->setMaximumSize(200 * w_fx, 200 * w_fx);
    
    ui->label_3->setMinimumSize(200 * w_fx, 24 * h_fx);
    ui->label_3->setMaximumSize(200 * w_fx, 24 * h_fx);
    font = ui->label_3->font();
    font.setPointSize(9 * w_fx);
    ui->label_3->setFont(font);
    
    ui->page2->setMinimumSize(310 * w_fx, 234 * h_fx);
    ui->page2->setMaximumSize(310 * w_fx, 234 * h_fx);
    
    ui->widget_cam->setMinimumSize(280 * w_fx, 210 * h_fx);
    ui->widget_cam->setMaximumSize(280 * w_fx, 210 * h_fx);
    
    ui->page3->setMinimumSize(310 * w_fx, 234 * h_fx);
    ui->page3->setMaximumSize(310 * w_fx, 234 * h_fx);
    
    ui->label_message->setMinimumSize(280 * w_fx, 24 * h_fx);
    ui->label_message->setMaximumSize(280 * w_fx, 24 * h_fx);
    font = ui->label_message->font();
    font.setPointSize(9 * w_fx);
    ui->label_message->setFont(font);
    
    ui->label_pixmap->setMinimumSize(280 * w_fx, 210 * h_fx);
    ui->label_pixmap->setMaximumSize(280 * w_fx, 210 * h_fx);
    
    ui->btn_register->setMinimumSize(100 * w_fx, 20 * h_fx);
    ui->btn_register->setMaximumSize(100 * w_fx, 20 * h_fx);
    
    ui->frame_2->setMinimumSize(310 * w_fx, 25 * h_fx);
    ui->frame_2->setMaximumSize(310 * w_fx, 25 * h_fx);
    
    ui->gridLayout_5->setContentsMargins(0, 0, 0, 5 * h_fx);
}

void firstLoginWidget::iniCamera()
{
    emit iniCam();
}

void firstLoginWidget::openCamera()
{
    emit openCam((HWND)ui->widget_cam->winId(), 
                 ui->widget_cam->width(), ui->widget_cam->height());
    
    emit startDetection();
}


void firstLoginWidget::on_btn_register_clicked()
{
    if(ui->stackedWidget->currentIndex() == 0)
    {
        showFloatingErrorWindow("请先录入人脸！");
        return;
    }
    ui->btn_register->setEnabled(false);
    
    if(ui->lineEdit_user->text().isEmpty())
    {
        ui->label_message->setText("请填写用户名");
        return;
    }
    else if(ui->lineEdit_pwd->text().isEmpty())
    {
        ui->label_message->setText("请填写密码");
        return;
    }
    else if(ui->lineEdit_department->text().isEmpty())
    {
        ui->label_message->setText("请填写部门");
        return;
    }
    else if(!detectionStatus)
    {
        ui->label_message->setText("请点击刷新按钮重新检测人脸");
        return;
    }
    
    if(!ui->btn_return->isVisible())
    {
        emit userRegistration(ui->lineEdit_user->text(), ui->lineEdit_pwd->text(), ui->lineEdit_department->text());
    }
    else
    {
        if(type == admin)
        {
            emit userReregistration(ui->lineEdit_user->text(), ui->lineEdit_pwd->text(), ui->lineEdit_department->text());
        }
        else if(type == tmpUser)
        {
            emit tmpUserRegistration(ui->lineEdit_user->text(), ui->lineEdit_pwd->text(), ui->lineEdit_department->text());
        }
        else if(type == changeFace)
        {
            emit changeTmpUserInfo(curUuid, ui->lineEdit_user->text(), ui->lineEdit_pwd->text(), ui->lineEdit_department->text());
        }
    }
}

void firstLoginWidget::on_btn_img_clicked()
{
    if(ui->lineEdit_status->text() != "相机初始化成功")
    {
        return;
    }
    ui->stackedWidget->setCurrentIndex(1);

//    iniCamera();
    openCamera();
}

void firstLoginWidget::onRepostCamMessage(QString messgae)
{
    ui->lineEdit_status->setText(messgae);
}

void firstLoginWidget::onSendPixmap(QPixmap pixmap)
{
    ui->stackedWidget->setCurrentIndex(2);
    
    QPixmap tmpPixmap = pixmap.scaled(ui->label_pixmap->width(), ui->label_pixmap->height(), 
                        Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->label_pixmap->setPixmap(tmpPixmap);
    ui->label_pixmap->setScaledContents(true);

    detectionStatus = true;
}

void firstLoginWidget::onSendVmMessage(QString message)
{
    ui->label_message->setText(message);
}

void firstLoginWidget::onSendRegisterResult(bool result)
{
    if(result && !ui->btn_return->isVisible())
    {
        emit success();
    }
    else if(result && ui->btn_return->isVisible())
    {
        emit savePwd();
    }
    else
    {
        ui->label_message->setText("注册失败，请重试");
        ui->btn_register->setEnabled(true);
    }
}

void firstLoginWidget::onSendTmpRegisterResult2(bool result, QString str)
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->btn_register->setEnabled(true);
    if(result)
    {
        curUuid.clear();
        curUsername.clear();
        
        if(type == tmpUser)
        {
            emit returnFaceManagePage2();
        }
        
        emit startSync();
//        else if(type == changeFace)
//        {
//            emit returnDetailPage();   
//        }
    }
    else
    {
        ui->lineEdit_status->setText("相机未初始化");
        if(str.isEmpty())
        {
            if(type == tmpUser)
            {
                showFloatingErrorWindow("注册失败");
            }
//            else if(type == changeFace)
//            {
//                showFloatingErrorWindow("变更人脸信息失败");
//            }
        }
        else
        {
            showFloatingErrorWindow(str);
        }
    }
}

void firstLoginWidget::onSendUpdateTmpUsrInfoRes(bool result, QString newUsername)
{
    qDebug()<<"onSendUpdateTmpUsrInfoRes";
    emit startSync();
    ui->stackedWidget->setCurrentIndex(0);
    ui->btn_register->setEnabled(true);
    if(result)
    {
        if(curUsername != newUsername)
        {
            emit delTmpUserEncFile(curUsername);
        }
        
        //变更信息成功后清除当前临时用户信息并返回人脸详情界面
        curUuid.clear();
        curUsername.clear();
        
        emit returnDetailPage();
    }
    else
    {
        //变更信息失败
        showFloatingErrorWindow("变更人脸信息失败");
    }
}

void firstLoginWidget::on_btn_refresh_clicked()
{
    if(ui->lineEdit_status->text() == "相机未初始化" || 
            (ui->lineEdit_status->text().isEmpty() && ui->lineEdit_status->placeholderText() == "相机未初始化"))
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

void firstLoginWidget::onShowReturnBtn()
{
    ui->btn_return->setVisible(true);
    ui->lineEdit_user->clear();
    ui->lineEdit_pwd->clear();
    ui->lineEdit_department->clear();
    ui->lineEdit_status->clear();
    ui->label_pixmap->clear();
    ui->stackedWidget->setCurrentIndex(0);
    ui->btn_register->setEnabled(true);
}

void firstLoginWidget::showFloatingErrorWindow(QString str)
{
    // 获取主窗口的 frameGeometry（包括边框和标题栏的绝对位置）
    QRect mainWindowRect = this->frameGeometry();     
    
    // 计算主窗口的中心位置
    int mainWindowCenterX = mainWindowRect.center().x();  // 主窗口中心 X 坐标
    int mainWindowTopY = mainWindowRect.top();  // 主窗口顶部 Y 坐标
    
    // 创建并显示错误窗口
    floatingErrorWindow* errorWindow = new floatingErrorWindow(str, nullptr);
    
    // 计算错误窗口的新位置，放置在主窗口的中心偏上
    int offsetFromTop = 50 * h_fx;  // 偏移量，可根据需要调整
    int errorWindowX = mainWindowCenterX - errorWindow->width() / 2;  // 水平居中
    int errorWindowY = mainWindowTopY + offsetFromTop;  // 垂直靠近顶部
    
    // 将错误窗口移动到计算出的相对坐标
    errorWindow->move(errorWindowX, errorWindowY);
    
    // 显示错误窗口
    errorWindow->show();
}

void firstLoginWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        whereismouse = event->pos();
    }
}

void firstLoginWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton)
    {
        //当窗口最大化或最小化时也不进行触发
        if (QWidget::isFullScreen() || QWidget::isMaximized() || QWidget::isMinimized())
        {
            return;
        }
        else
        {
            //当在按钮之类需要鼠标操作的地方不进行触发(防误触)
//            if (ui->btn_close->underMouse() || ui->btn_min->underMouse() || ui->btn_return->underMouse()
//                    || ui->frame->underMouse() || ui->frame_2->underMouse() || ui->frame_3->underMouse())
            if(ui->frame_title->underMouse())
            {
                if(ui->btn_close->underMouse() || ui->btn_min->underMouse() || ui->btn_return->underMouse())
                {
                    
                }
                else
                {
                    QWidget::move(QWidget::mapToGlobal(event->pos() - whereismouse));//移动
                }
            }
        }
    }
    event->accept();
}

void firstLoginWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) 
    {
        ui->btn_register->click();
    }
    QWidget::keyPressEvent(event);
}

void firstLoginWidget::setUsername(QString username)
{
    ui->lineEdit_user->setText(username);
}

void firstLoginWidget::on_btn_return_clicked()
{
    curUuid.clear();
    curUsername.clear();
    
    emit uniniCam();
    
    if(type == admin)
    {
        emit returnHomePage();
    }
    else if(type == tmpUser)
    {
        emit returnFaceManagePage();
    }
    else if(type == changeFace)
    {
        emit returnDetailPage2();
    }
}

void firstLoginWidget::on_btn_min_clicked()
{
    QWidget::showMinimized();
}

void firstLoginWidget::on_btn_close_clicked()
{
    QWidget::close();
}
