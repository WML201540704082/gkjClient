#include "pwdloginwidget.h"
#include "ui_pwdloginwidget.h"
#include "globalVariables.h"
#include "floatingerrorwindow.h"
#include "utf8.h"

#include <QScreen>
#include <QMouseEvent>

pwdLoginWidget::pwdLoginWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::pwdLoginWidget)
{
    ui->setupUi(this);
    iniUi();
    screenAdpater();
}

pwdLoginWidget::~pwdLoginWidget()
{
    delete ui;
}

void pwdLoginWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        whereismouse = event->pos();
    }
}

void pwdLoginWidget::mouseMoveEvent(QMouseEvent *event)
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
                    || ui->frame->underMouse() || ui->frame_2->underMouse() || ui->frame_3->underMouse())
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

void pwdLoginWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return  || event->key() == Qt::Key_Enter) 
    {
        ui->btn->click();
    }
    QWidget::keyPressEvent(event);
}

void pwdLoginWidget::iniUi()
{
    //去掉边框
    setWindowFlag(Qt::FramelessWindowHint);
    //窗口半透明（因为背景图片采用外阴影，使用该选项才能去除窗口无色边框）
    setAttribute(Qt::WA_TranslucentBackground);
}

void pwdLoginWidget::screenAdpater()
{
    if(w_fx == 0 || h_fx ==0)
    {
        QScreen* pScreen = QGuiApplication::primaryScreen();
        QRect rect1 = pScreen->geometry();

        w_fx = (double)rect1.size().width() / 1920.0;
        h_fx = (double)rect1.size().height() / 1080.0;
    }

//    this->setFixedSize(928 * w_fx, 556 * h_fx);
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
    
    ui->frame->setMinimumSize(550 * w_fx, 360 * h_fx);
    ui->frame->setMaximumSize(550 * w_fx, 360 * h_fx);
    
    ui->frame_2->setMinimumSize(550 * w_fx, 180 * h_fx);
    ui->frame_2->setMaximumSize(550 * w_fx, 180 * h_fx);
    
    ui->frame_3->setMinimumSize(550 * w_fx, 180 * h_fx);
    ui->frame_3->setMaximumSize(550 * w_fx, 180 * h_fx);
    
    ui->label_pwd->setMinimumSize(100 * w_fx, 22 * h_fx);
    ui->label_pwd->setMaximumSize(100 * w_fx, 22 * h_fx);
    font = ui->label_pwd->font();
    font.setPointSize(8 * w_fx);
    ui->label_pwd->setFont(font);
    
    ui->lineEdit_pwd->setMinimumSize(240 * w_fx, 22 * h_fx);
    ui->lineEdit_pwd->setMaximumSize(240 * w_fx, 22 * h_fx);
    font = ui->lineEdit_pwd->font();
    font.setPointSize(8 * w_fx);
    ui->lineEdit_pwd->setFont(font);
    
    ui->btn->setMinimumSize(206 * w_fx, 48 * h_fx);
    ui->btn->setMaximumSize(206 * w_fx, 48 * h_fx);
    font = ui->btn->font();
    font.setPointSize(9 * w_fx);
    ui->btn->setFont(font);
}

void pwdLoginWidget::showFloatingErrorWindow(QString str)
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

void pwdLoginWidget::onSendPwdCheckResult(bool result)
{
    if(result)
    {
        emit checkPassed();
    }
    else
    {
        showFloatingErrorWindow("密码错误，请重试或人脸登录");
    }
}

void pwdLoginWidget::on_btn_return_clicked()
{
    emit returnFaceLoginPage();
}

void pwdLoginWidget::on_btn_min_clicked()
{
    QWidget::showMinimized();
}

void pwdLoginWidget::on_btn_close_clicked()
{
    QWidget::close();
}

void pwdLoginWidget::on_btn_clicked()
{
    QString pwdStr = ui->lineEdit_pwd->text();
    if(pwdStr.isEmpty())
    {
        return;
    }
    
    emit checkUserPwd(pwdStr);
}
