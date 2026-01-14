#include "timewidget.h"
#include "ui_timewidget.h"
#include "globalVariables.h"
#include "floatingerrorwindow.h"

#include <QScreen>
#include <QMouseEvent>
#include <QDebug>

timeWidget::timeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::timeWidget)
{
    ui->setupUi(this);
    iniUi();
    screenAdpater();
}

timeWidget::~timeWidget()
{
    delete ui;
}

void timeWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        whereismouse = event->pos();
    }
}

void timeWidget::mouseMoveEvent(QMouseEvent *event)
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
                    || ui->frame->underMouse() || ui->frame_3->underMouse())
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

void timeWidget::iniUi()
{
    //去掉边框
    setWindowFlag(Qt::FramelessWindowHint);
    //窗口半透明（因为背景图片采用外阴影，使用该选项才能去除窗口无色边框）
    setAttribute(Qt::WA_TranslucentBackground);
    
    ui->dateTimeEdit_start->setDateTime(QDateTime::currentDateTime());
    ui->dateTimeEdit_start->setEnabled(true);
    ui->dateTimeEdit_end->setDateTime(QDateTime::currentDateTime());
    ui->dateTimeEdit_end->setEnabled(true);
}

void timeWidget::screenAdpater()
{
    if(w_fx == 0 || h_fx ==0)
    {
        QScreen* pScreen = QGuiApplication::primaryScreen();
        QRect rect1 = pScreen->geometry();

        w_fx = (double)rect1.size().width() / 1920.0;
        h_fx = (double)rect1.size().height() / 1080.0;
    }
    
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
    
    ui->gridLayout_4->setContentsMargins(9 * w_fx, 9 * h_fx, 9 * w_fx, 9 * h_fx);
    ui->gridLayout_4->setVerticalSpacing(6 * h_fx);
    ui->gridLayout_4->setHorizontalSpacing(6 * w_fx);
    
    ui->frame_2->setMaximumSize(530 * w_fx, 300 * h_fx);
    ui->frame_2->setMinimumSize(530 * w_fx, 300 * h_fx);
    
    ui->horizontalLayout->setContentsMargins(5 * w_fx, 5 * h_fx, 5 * w_fx, 5 * h_fx);
    
    ui->frame_5->setMaximumSize(240 * w_fx, 224 * h_fx);
    ui->frame_5->setMinimumSize(240 * w_fx, 224 * h_fx);   

    ui->label_face->setMaximumSize(240 * w_fx, 224 * h_fx);
    ui->label_face->setMinimumSize(240 * w_fx, 224 * h_fx);
    
    ui->frame_3->setMaximumSize(280 * w_fx, 224 * h_fx);
    ui->frame_3->setMinimumSize(280 * w_fx, 224 * h_fx);
    
    ui->gridLayout_2->setContentsMargins(9 * w_fx, 0, 9 * w_fx, 0);
    ui->gridLayout_2->setVerticalSpacing(6 * h_fx);
    ui->gridLayout_2->setHorizontalSpacing(6 * w_fx);
    
    ui->label_2->setMinimumSize(70 * w_fx, 40 * h_fx);
    ui->label_2->setMaximumSize(70 * w_fx, 40 * h_fx);
    font = ui->label_2->font();
    font.setPointSize(8 * w_fx);
    ui->label_2->setFont(font);
    
    ui->label_3->setMinimumSize(70 * w_fx, 40 * h_fx);
    ui->label_3->setMaximumSize(70 * w_fx, 40 * h_fx);
    ui->label_3->setFont(font);
    
    ui->label_4->setMinimumSize(70 * w_fx, 40 * h_fx);
    ui->label_4->setMaximumSize(70 * w_fx, 40 * h_fx);
    ui->label_4->setFont(font);
    
    ui->lineEdit_user->setMinimumSize(180 * w_fx, 40 * h_fx);
    ui->lineEdit_user->setMaximumSize(180 * w_fx, 40 * h_fx);
    ui->lineEdit_user->setFont(font);
    
    ui->dateTimeEdit_start->setMinimumSize(180 * w_fx, 40 * h_fx);
    ui->dateTimeEdit_start->setMaximumSize(180 * w_fx, 40 * h_fx);
    ui->dateTimeEdit_start->setFont(font);
    
    ui->dateTimeEdit_end->setMinimumSize(180 * w_fx, 40 * h_fx);
    ui->dateTimeEdit_end->setMaximumSize(180 * w_fx, 40 * h_fx);
    ui->dateTimeEdit_end->setFont(font);
    
    ui->frame_4->setMinimumSize(257 * w_fx, 50 * h_fx);
    ui->frame_4->setMaximumSize(257 * w_fx, 50 * h_fx);
    
    ui->gridLayout_3->setContentsMargins(9 * w_fx, 9 * h_fx, 9 * w_fx, 9 * h_fx);
    ui->gridLayout_3->setVerticalSpacing(6 * h_fx);
    ui->gridLayout_3->setHorizontalSpacing(6 * w_fx);
    
    ui->pushButton_3->setMinimumSize(150 * w_fx, 30 * h_fx);
    ui->pushButton_3->setMaximumSize(150 * w_fx, 30 * h_fx);
    ui->pushButton_3->setFont(font);
}

void timeWidget::setUsername(QString username)
{
    ui->lineEdit_user->setText(username);
}

void timeWidget::setPixmap(QPixmap tmpPixmap)
{
    QPixmap pixmap = tmpPixmap.scaled(ui->label_face->width(), ui->label_face->height(), 
                                      Qt::IgnoreAspectRatio, Qt::SmoothTransformation); 
    
    ui->label_face->setPixmap(pixmap);
    ui->label_face->setScaledContents(true);
}

void timeWidget::setDateTime(QString startTime, QString endTime)
{
    oldStartTime = startTime;
    oldEndTime = endTime;
    ui->dateTimeEdit_start->setDateTime(QDateTime::fromString(startTime, "yyyy-MM-dd HH:mm"));
    ui->dateTimeEdit_start->setEnabled(true);
    ui->dateTimeEdit_end->setDateTime(QDateTime::fromString(endTime, "yyyy-MM-dd HH:mm"));
    ui->dateTimeEdit_end->setEnabled(true);
}

void timeWidget::setDateTime()
{
    ui->dateTimeEdit_start->setDateTime(QDateTime::currentDateTime());
    ui->dateTimeEdit_start->setEnabled(true);
    ui->dateTimeEdit_end->setDateTime(QDateTime::currentDateTime().addMonths(1));
    ui->dateTimeEdit_end->setEnabled(true);
}

void timeWidget::showFloatingErrorWindow(QString str)
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

void timeWidget::onSendAddUserPeriodRes(bool result)
{
    ui->pushButton_3->setEnabled(true);
    if(result)
    {
        emit startSync();
        emit addUserPeriodSuccess();
    }
    else
    {
        showFloatingErrorWindow("添加失败，请重试！");
    }
}

void timeWidget::onSendChangeUserPeriodRes(bool result)
{
    ui->pushButton_3->setEnabled(true);
    if(result)
    {
        emit startSync();
        if(curType == change1)
        {
            emit changeUserPeriodSuccess();
        }
        else if(curType == change2)
        {
            emit changeUserPeriodSuccess2();
        }
    }
    else
    {
        showFloatingErrorWindow("更新失败，请重试！");
    }
}

void timeWidget::on_btn_min_clicked()
{
    QWidget::showMinimized();
}

void timeWidget::on_btn_close_clicked()
{
    QWidget::close();
}

void timeWidget::on_btn_return_clicked()
{
    if(curType == add || curType == change1)
    {
        emit returnDetailPage();
    }
    else if(curType == change2)
    {
        emit returnAuthManagePage();
    }
}

void timeWidget::on_pushButton_3_clicked()
{
    if(ui->lineEdit_user->text().isEmpty())
    {
        showFloatingErrorWindow("请填写用户名");
    }
    else if(ui->dateTimeEdit_start->dateTime() >= ui->dateTimeEdit_end->dateTime())
    {
        showFloatingErrorWindow("起止时间范围错误");
    }
    else
    {
        ui->pushButton_3->setEnabled(false);
        QString startTime = ui->dateTimeEdit_start->dateTime().toString("yyyy-MM-dd hh:mm");
        QString endTime = ui->dateTimeEdit_end->dateTime().toString("yyyy-MM-dd hh:mm");
        if(curType == add)
        {
            emit addUserPeriod(curUuid, startTime, endTime);
        }
        else
        {
            emit changeUserPeriod(curUuid, oldStartTime, oldEndTime, startTime, endTime);
        } 
    }
}
