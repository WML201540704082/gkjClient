#include "faceloginwidget.h"
#include "ui_faceloginwidget.h"
#include "globalVariables.h"
#include "floatingerrorwindow.h"
#include "utf8.h"

#include <QScreen>
#include <QBitmap>
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>

faceLoginWidget::faceLoginWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::faceLoginWidget)
{
    ui->setupUi(this);
    
    iniUI();
    screenAdapt();
}

faceLoginWidget::~faceLoginWidget()
{
    delete ui;
}

void faceLoginWidget::iniUI()
{
    //去掉边框
    setWindowFlag(Qt::FramelessWindowHint);
}

void faceLoginWidget::screenAdapt()
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
    
    ui->btn_min->setMinimumSize(20 * w_fx, 20 * w_fx);
    ui->btn_min->setMaximumSize(20 * w_fx, 20 * w_fx);
    
    ui->btn_close->setMinimumSize(20 * w_fx, 20 * w_fx);
    ui->btn_close->setMaximumSize(20 * w_fx, 20 * w_fx);
    
    ui->stackedWidget->setMinimumSize(310 * w_fx, 250 * h_fx);
    ui->stackedWidget->setMaximumSize(310 * w_fx, 250 * h_fx);
    
    ui->page1->setMinimumSize(310 * w_fx, 250 * h_fx);
    ui->page1->setMaximumSize(310 * w_fx, 250 * h_fx);
    
    ui->gridLayout_2->setContentsMargins(9 * w_fx, 9 * h_fx, 9 * w_fx, 9 * h_fx);
    ui->gridLayout_2->setHorizontalSpacing(6 * w_fx);
    ui->gridLayout_2->setVerticalSpacing(6 * h_fx);
    
    ui->btn_img->setMinimumSize(200 * w_fx, 200 * w_fx);
    ui->btn_img->setMaximumSize(200 * w_fx, 200 * w_fx);
    
    ui->label_3->setMinimumSize(200 * w_fx, 24 * h_fx);
    ui->label_3->setMaximumSize(200 * w_fx, 24 * h_fx);
    font = ui->label_3->font();
    font.setPointSize(9 * w_fx);
    ui->label_3->setFont(font);
    
    ui->page2->setMinimumSize(310 * w_fx, 250 * h_fx);
    ui->page2->setMaximumSize(310 * w_fx, 250 * h_fx);
    
    ui->widget_cam->setMinimumSize(280 * w_fx, 210 * h_fx);
    ui->widget_cam->setMaximumSize(280 * w_fx, 210 * h_fx);
    
    ui->page3->setMinimumSize(310 * w_fx, 250 * h_fx);
    ui->page3->setMaximumSize(310 * w_fx, 250 * h_fx);
    
    ui->label_message->setMinimumSize(280 * w_fx, 38 * h_fx);
    ui->label_message->setMaximumSize(280 * w_fx, 38 * h_fx);
    
    ui->label_pixmap->setMinimumSize(280 * w_fx, 210 * h_fx);
    ui->label_pixmap->setMaximumSize(280 * w_fx, 210 * h_fx);
    
    ui->btn_pwd->setMinimumSize(100 * w_fx, 20 * h_fx);
    ui->btn_pwd->setMaximumSize(100 * w_fx, 20 * h_fx);
    font = ui->btn_pwd->font();
    font.setPointSize(9 * w_fx);
    ui->btn_pwd->setFont(font);
    
    ui->frame_2->setMinimumSize(310 * w_fx, 40 * h_fx);
    ui->frame_2->setMaximumSize(310 * w_fx, 40 * h_fx);
}

void faceLoginWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        whereismouse = event->pos();
    }
}

void faceLoginWidget::mouseMoveEvent(QMouseEvent *event)
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
                if(ui->btn_close->underMouse() || ui->btn_min->underMouse())
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

void faceLoginWidget::iniCamera()
{
    emit iniCam();
}

void faceLoginWidget::openCamera()
{
    emit openCam((HWND)ui->widget_cam->winId(), 
                 ui->widget_cam->width(), ui->widget_cam->height());
    
    emit startDetection();
}

void faceLoginWidget::showFloatingErrorWindow(QString str)
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

void faceLoginWidget::onRepostCamMessage(QString message)
{
    if(!this->isVisible())
    {
        return;
    }
    
    if(message != "相机初始化成功")
    {
        if(message == "检测超时，请点击按钮重试！")
        {
            message = "检测超时，请点击图片重试！";
            timeout = true;
        }
        showFloatingErrorWindow(message);
        ui->stackedWidget->setCurrentIndex(0);
    }
    else
    {
        camIni = true;
    }
}

void faceLoginWidget::onSendVmMessage(QString message)
{
    if(message != "处理人脸图片成功")
    {
        showFloatingErrorWindow(message);
    }   
}

void faceLoginWidget::onSendPixmap(QPixmap pixmap)
{
    if(ui->stackedWidget->currentIndex() == 0)
    {
        return;
    }
    ui->stackedWidget->setCurrentIndex(2);
    
    ui->label_pixmap->setPixmap(pixmap);
    ui->label_pixmap->setScaledContents(true);
    
    emit checkUserFace();
}

void faceLoginWidget::onSendUserIdentity(QString identityStr)
{
    if(identityStr == "admin")
    {
        ui->stackedWidget->setCurrentIndex(0);
        emit checkPassed();
    }
    else if(identityStr == "notAdmin")
    {
        ui->stackedWidget->setCurrentIndex(0);
        showFloatingErrorWindow("非管理员人脸，请重试或密码登录");
    }
}

void faceLoginWidget::on_btn_pwd_clicked()
{
    emit uniniCam();
    camIni = false;
    emit showPwdLoginPage();
    ui->stackedWidget->setCurrentIndex(0);
}

void faceLoginWidget::on_btn_img_clicked()
{
    if(!camIni)
    {
        showFloatingErrorWindow("相机初始化未完成");
        return;
    }
    
    ui->stackedWidget->setCurrentIndex(1);
    
    if(timeout)
    {
        timeout = false;
        emit startDetection();
    }
    else
    {
        //    iniCamera();
        openCamera();
    }
}

void faceLoginWidget::on_btn_min_clicked()
{
    QWidget::showMinimized();
}

void faceLoginWidget::on_btn_close_clicked()
{
    QWidget::close();
}
