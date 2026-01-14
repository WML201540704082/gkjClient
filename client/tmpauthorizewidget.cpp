#include "tmpauthorizewidget.h"
#include "ui_tmpauthorizewidget.h"
#include "floatingerrorwindow.h"
#include "utf8.h"
#include "globalVariables.h"

#include <QDebug>
#include <QMessageBox>
#include <QScreen>
#include <QBitmap>
#include <QPainter>
#include <QFont>
#include <QMouseEvent>

tmpAuthorizeWidget::tmpAuthorizeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::tmpAuthorizeWidget)
{
    ui->setupUi(this);
    
    iniUI();
    screenAdapter();
}

tmpAuthorizeWidget::~tmpAuthorizeWidget()
{
    delete ui;
}

void tmpAuthorizeWidget::iniUI()
{
    //去掉边框
    setWindowFlag(Qt::FramelessWindowHint);
    
    ui->dateTimeEdit_start->setDateTime(QDateTime::currentDateTime());
    ui->dateTimeEdit_start->setEnabled(true);
    ui->dateTimeEdit_end->setDateTime(QDateTime::currentDateTime().addMonths(1));
    ui->dateTimeEdit_end->setEnabled(true);
    ui->lineEdit_user->clear();
    ui->lineEdit_user->setEnabled(true);
    ui->lineEdit_pwd->clear();
    ui->lineEdit_pwd->setEnabled(true);
    ui->lineEdit_department->clear();
    ui->lineEdit_department->setEnabled(true);
    
    ui->stackedWidget->setCurrentIndex(0);
    
    detectionStatus = false;
}

void tmpAuthorizeWidget::screenAdapter()
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
    
    this->setMinimumSize(550 * w_fx, 400 * h_fx);
    this->setMaximumSize(550 * w_fx, 400 * h_fx);
    
    ui->widget->setMinimumSize(550 * w_fx, 400 * h_fx);
    ui->widget->setMaximumSize(550 * w_fx, 400 * h_fx);
    
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
    
    ui->frame->setMinimumSize(550 * w_fx, 270 * h_fx);
    ui->frame->setMaximumSize(550 * w_fx, 270 * h_fx);
    
    ui->horizontalLayout_3->setContentsMargins(9 * w_fx, 9 * h_fx, 9 * w_fx, 9 * h_fx);
    ui->horizontalLayout_3->setSpacing(6 * w_fx);
    
    ui->stackedWidget->setMinimumSize(240 * w_fx, 240 * w_fx);
    ui->stackedWidget->setMaximumSize(240 * w_fx, 240 * w_fx);
    
    ui->page->setMinimumSize(240 * w_fx, 240 * w_fx);
    ui->page->setMaximumSize(240 * w_fx, 240 * w_fx);
    
    ui->label->setMinimumSize(230 * w_fx, 230 * w_fx);
    ui->label->setMaximumSize(230 * w_fx, 230 * w_fx);
    
    ui->page_2->setMinimumSize(240 * w_fx, 240 * w_fx);
    ui->page_2->setMaximumSize(240 * w_fx, 240 * w_fx);
    
    ui->widget_cam->setMinimumSize(240 * w_fx, 240 * w_fx);
    ui->widget_cam->setMaximumSize(240 * w_fx, 240 * w_fx);
    
    ui->page_3->setMinimumSize(240 * w_fx, 240 * w_fx);
    ui->page_3->setMaximumSize(240 * w_fx, 240 * w_fx);
    
    ui->label_pixmap->setMinimumSize(240 * w_fx, 224 * h_fx);
    ui->label_pixmap->setMaximumSize(240 * w_fx, 224 * h_fx);
    
    ui->frame_3->setMinimumSize(280 * w_fx, 240 * h_fx);
    ui->frame_3->setMaximumSize(280 * w_fx, 240 * h_fx);
    
    ui->gridLayout_2->setContentsMargins(9 * w_fx, 9 * h_fx, 9 * w_fx, 9 * h_fx);
    ui->gridLayout_2->setHorizontalSpacing(6 * w_fx);
    ui->gridLayout_2->setVerticalSpacing(6 * h_fx);
    
    ui->label_2->setMinimumSize(70 * w_fx, 40 * h_fx);
    ui->label_2->setMaximumSize(70 * w_fx, 40 * h_fx);
    font = ui->label_2->font();
    font.setPointSize(7 * w_fx);
    ui->label_2->setFont(font);
    
    ui->label_3->setMinimumSize(70 * w_fx, 40 * h_fx);
    ui->label_3->setMaximumSize(70 * w_fx, 40 * h_fx);
    ui->label_3->setFont(font);
    
    ui->label_4->setMinimumSize(70 * w_fx, 40 * h_fx);
    ui->label_4->setMaximumSize(70 * w_fx, 40 * h_fx);
    ui->label_4->setFont(font);
    
    ui->label_5->setMinimumSize(70 * w_fx, 40 * h_fx);
    ui->label_5->setMaximumSize(70 * w_fx, 40 * h_fx);
    ui->label_5->setFont(font);
    
    ui->label_6->setMinimumSize(70 * w_fx, 40 * h_fx);
    ui->label_6->setMaximumSize(70 * w_fx, 40 * h_fx);
    ui->label_6->setFont(font);
    
    ui->lineEdit_user->setMinimumSize(180 * w_fx, 40 * h_fx);
    ui->lineEdit_user->setMaximumSize(180 * w_fx, 40 * h_fx);
    ui->lineEdit_user->setFont(font);
    
    ui->lineEdit_pwd->setMinimumSize(180 * w_fx, 40 * h_fx);
    ui->lineEdit_pwd->setMaximumSize(180 * w_fx, 40 * h_fx);
    ui->lineEdit_pwd->setFont(font);
    
    ui->lineEdit_department->setMinimumSize(180 * w_fx, 40 * h_fx);
    ui->lineEdit_department->setMaximumSize(180 * w_fx, 40 * h_fx);
    ui->lineEdit_department->setFont(font);
    
    ui->dateTimeEdit_start->setMinimumSize(180 * w_fx, 40 * h_fx);
    ui->dateTimeEdit_start->setMaximumSize(180 * w_fx, 40 * h_fx);
    ui->dateTimeEdit_start->setFont(font);
    
    ui->dateTimeEdit_end->setMinimumSize(180 * w_fx, 40 * h_fx);
    ui->dateTimeEdit_end->setMaximumSize(180 * w_fx, 40 * h_fx);
    ui->dateTimeEdit_end->setFont(font);
    
    ui->frame_2->setMinimumSize(550 * w_fx, 90 * h_fx);
    ui->frame_2->setMaximumSize(550 * w_fx, 90 * h_fx);
    
    ui->horizontalLayout->setContentsMargins(9 * w_fx, 9 * h_fx, 9 * w_fx, 9 * h_fx);
    ui->horizontalLayout->setSpacing(6 * w_fx);
    
    ui->pushButton->setMinimumSize(150 * w_fx, 30 * h_fx);
    ui->pushButton->setMaximumSize(150 * w_fx, 30 * h_fx);
    ui->pushButton->setFont(font);
    
    ui->pushButton_2->setMinimumSize(150 * w_fx, 30 * h_fx);
    ui->pushButton_2->setMaximumSize(150 * w_fx, 30 * h_fx);
    ui->pushButton_2->setFont(font);
    
    ui->pushButton_3->setMinimumSize(150 * w_fx, 30 * h_fx);
    ui->pushButton_3->setMaximumSize(150 * w_fx, 30 * h_fx);
    ui->pushButton_3->setFont(font);
}

void tmpAuthorizeWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        whereismouse = event->pos();
    }
}

void tmpAuthorizeWidget::mouseMoveEvent(QMouseEvent *event)
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
            if (ui->btn_close->underMouse() || ui->btn_min->underMouse() || ui->btn_return->underMouse()
                    || ui->frame->underMouse() || ui->frame_2->underMouse())
            {
            }
            else
            {
                QWidget::move(QWidget::mapToGlobal(event->pos() - whereismouse));//移动
            }
        }
    }
    event->accept();
}

void tmpAuthorizeWidget::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::WindowStateChange)
    {                
        if(!isMinimized())
        {
            setAttribute(Qt::WA_Mapped);
        }
    }      
    
    QWidget::changeEvent(event);
    
}

void tmpAuthorizeWidget::showFloatingErrorWindow(QString str)
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

void tmpAuthorizeWidget::iniCamera()
{
    emit iniCam();
}

void tmpAuthorizeWidget::openCamera()
{
    emit openCam((HWND)ui->widget_cam->winId(), 
                 ui->widget_cam->width(), ui->widget_cam->height());
    
    emit startDetection();
}

void tmpAuthorizeWidget::setPixmap(QPixmap pixmap)
{
    ui->stackedWidget->setCurrentIndex(2);
    QPixmap tmpPixmap = pixmap.scaled(ui->label_pixmap->width(), ui->label_pixmap->height(), 
                                      Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->label_pixmap->setPixmap(tmpPixmap);
    ui->label_pixmap->setScaledContents(true);
}

void tmpAuthorizeWidget::setUsername(QString username)
{
    ui->lineEdit_user->setText(username);
    ui->lineEdit_user->setEnabled(false);
}

void tmpAuthorizeWidget::onSendPixmap(QPixmap pixmap)
{
    if(ui->stackedWidget->currentIndex() == 0)
    {
        return;
    }
    
    ui->pushButton->setEnabled(true);
    
    ui->stackedWidget->setCurrentIndex(2);
    
    ui->label_pixmap->setPixmap(pixmap);
    ui->label_pixmap->setScaledContents(true);
    
    detectionStatus = true;
}

void tmpAuthorizeWidget::onRepostCamMessage(QString message)
{
    if(!this->isVisible())
    {
        return;
    }
    if(message != "相机初始化成功")
    {
        ui->pushButton->setEnabled(true);
        showFloatingErrorWindow(message);
        if(message == "检测超时，请点击按钮重试！")
        {
            timeout = true;
        }
    }
    else
    {
        camIni = true;
    }
}

void tmpAuthorizeWidget::onSendVmMessage(QString message)
{
    if(message != "处理人脸图片成功")
    {
        showFloatingErrorWindow(message);
    }   
}

void tmpAuthorizeWidget::onSendTmpRegisterResult(bool result, QString str)
{
    if(!result)
    {
        camOpen = false;
        if(!str.isEmpty())
        {
            QMessageBox::warning(nullptr, "错误", str);
        }
        else
        {
            QMessageBox::warning(nullptr, "错误", "注册临时用户失败，请重试");
        }   
    }
    else
    {
        QMessageBox::information(0, "提示", "注册临时用户成功");
        detectionStatus = false;
        emit startSync();
    }
    
    ui->stackedWidget->setCurrentIndex(0);
    ui->lineEdit_user->clear();
    ui->lineEdit_pwd->clear();
    ui->lineEdit_department->clear();
    ui->dateTimeEdit_start->setDateTime(QDateTime::currentDateTime());
    ui->dateTimeEdit_end->setDateTime(QDateTime::currentDateTime());
}

void tmpAuthorizeWidget::on_pushButton_clicked()
{
    if(!camIni)
    {
        showFloatingErrorWindow("相机初始化未完成");
        return;
    }
    
    ui->pushButton->setEnabled(false);
    
    if(timeout && !camOpen)
    {
        timeout = false;
        emit startDetection();
    }
    else if(!timeout && camOpen)
    {
        ui->stackedWidget->setCurrentIndex(1);
        emit startDetection();
    }
    else
    {
        ui->stackedWidget->setCurrentIndex(1);
        camOpen = true;
        
    //    iniCamera();
        openCamera();
    }
    
}

void tmpAuthorizeWidget::on_pushButton_3_clicked()
{
    if(ui->lineEdit_user->text().isEmpty())
    {
        showFloatingErrorWindow("请填写用户名");
    }
    else if(ui->lineEdit_pwd->text().isEmpty())
    {
        showFloatingErrorWindow("请填写密码");
    }
    else if(ui->dateTimeEdit_start->dateTime() >= ui->dateTimeEdit_end->dateTime())
    {
        showFloatingErrorWindow("起止时间范围错误");
    }
    else if(ui->lineEdit_department->text().isEmpty())
    {
        showFloatingErrorWindow("请填写部门");
    }
    else if(!detectionStatus)
    {
        showFloatingErrorWindow("未完成人脸检测");
    }
    else
    {
        QString username = ui->lineEdit_user->text();
        QString userPwd = ui->lineEdit_pwd->text();
        QString startTime = ui->dateTimeEdit_start->dateTime().toString("yyyy-MM-dd hh:mm");
        QString endTime = ui->dateTimeEdit_end->dateTime().toString("yyyy-MM-dd hh:mm");
        QString department = ui->lineEdit_department->text();
        emit tmpUserRegistration(username, userPwd, startTime, endTime, department);
    }
}

void tmpAuthorizeWidget::on_btn_min_clicked()
{
    QWidget::showMinimized();
}

void tmpAuthorizeWidget::on_btn_close_clicked()
{
    QWidget::close();
}

void tmpAuthorizeWidget::on_btn_return_clicked()
{
    emit uniniCam();
    camIni = false;
    camOpen = false;
    emit returnAuthManagePage();
}

void tmpAuthorizeWidget::on_pushButton_2_clicked()
{
    emit chooseFace();
}
