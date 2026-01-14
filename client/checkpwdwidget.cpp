#include "checkpwdwidget.h"
#include "ui_checkpwdwidget.h"
#include "utf8.h"
#include "globalVariables.h"

#include <QAction>
#include <QScreen>
#include <QFont>
#include <QDebug>
#include <QDir>
#include <QMouseEvent>

checkPwdWidget::checkPwdWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::checkPwdWidget)
{
    ui->setupUi(this);

    iniUI();
    screenAdapt();
}

checkPwdWidget::~checkPwdWidget()
{
    delete ui;
}

void checkPwdWidget::iniUI()
{
    //去掉边框
    setWindowFlag(Qt::FramelessWindowHint);
    //窗口半透明（因为背景图片采用外阴影，使用该选项才能去除窗口无色边框）
    setAttribute(Qt::WA_TranslucentBackground);
    
    QAction *action = new QAction(this);
    action->setIcon(QIcon(":/images/images/user.png"));
    ui->lineEdit_account->addAction(action,QLineEdit::LeadingPosition);
    ui->lineEdit_account->setPlaceholderText("请输入操作系统账号");
    QDir dir;
//    QString userName = dir.home().dirName();
    QString userName = qgetenv("USERNAME");
    ui->lineEdit_account->setText(userName);

    QAction *action2 = new QAction(this);
    action2->setIcon(QIcon(":/images/images/password.png"));
    ui->lineEdit_pwd->addAction(action2,QLineEdit::LeadingPosition);
    ui->lineEdit_pwd->setPlaceholderText("请输入操作系统密码");
    
    ui->btn_return->setVisible(false);
    ui->btn_return->setCursor(QCursor(Qt::PointingHandCursor));
}

void checkPwdWidget::screenAdapt()
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
    
    ui->frame->setMinimumSize(550 * w_fx, 57 * h_fx);
    ui->frame->setMaximumSize(550 * w_fx, 57 * h_fx);
    
    ui->horizontalLayout_5->setContentsMargins(9 * w_fx, 9 * h_fx, 9 * w_fx, 9 * h_fx);
    ui->horizontalLayout_5->setSpacing(6 * w_fx);
    
    ui->label->setMinimumSize(224 * w_fx, 37 * h_fx);
    ui->label->setMaximumSize(224 * w_fx, 37 * h_fx);
    font = ui->label->font();
    font.setPointSize(13 * w_fx);
    ui->label->setFont(font);
    
    ui->label_2->setMinimumSize(60 * w_fx, 10 * h_fx);
    ui->label_2->setMaximumSize(60 * w_fx, 10 * h_fx);
    
    ui->label_3->setMinimumSize(60 * w_fx, 10 * h_fx);
    ui->label_3->setMaximumSize(60 * w_fx, 10 * h_fx);
    
    ui->frame_2->setMinimumSize(550 * w_fx, 135 * h_fx);
    ui->frame_2->setMaximumSize(550 * w_fx, 135 * h_fx);
    
    ui->gridLayout->setContentsMargins(9 * w_fx, 50 * h_fx, 9 * w_fx, 0);
    ui->gridLayout->setHorizontalSpacing(6 * w_fx);
    ui->gridLayout->setVerticalSpacing(39 * w_fx);
    
    ui->label_account->setMinimumSize(100 * w_fx, 22 * h_fx);
    ui->label_account->setMaximumSize(100 * w_fx, 22 * h_fx);
    font = ui->label_account->font();
    font.setPointSize(8 * w_fx);
    ui->label_account->setFont(font);
    
    ui->lineEdit_account->setMinimumSize(240 * w_fx, 22 * h_fx);
    ui->lineEdit_account->setMaximumSize(240 * w_fx, 22 * h_fx);
    font = ui->lineEdit_account->font();
    font.setPointSize(8 * w_fx);
    ui->lineEdit_account->setFont(font);
    
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
    
    ui->frame_3->setMinimumSize(550 * w_fx, 168 * h_fx);
    ui->frame_3->setMaximumSize(550 * w_fx, 168 * h_fx);
    
    ui->gridLayout_4->setContentsMargins(9 * w_fx, 9 * h_fx, 9 * w_fx, 9 * h_fx);
    ui->gridLayout_4->setHorizontalSpacing(6 * w_fx);
    ui->gridLayout_4->setVerticalSpacing(6 * h_fx);
    
    ui->label_res->setMinimumSize(206 * w_fx, 22 * h_fx);
    ui->label_res->setMaximumSize(206 * w_fx, 22 * h_fx);
    
    ui->btn->setMinimumSize(206 * w_fx, 48 * h_fx);
    ui->btn->setMaximumSize(206 * w_fx, 48 * h_fx);
    font = ui->btn->font();
    font.setPointSize(9 * w_fx);
    ui->btn->setFont(font);
}

void checkPwdWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        whereismouse = event->pos();
    }
}

void checkPwdWidget::mouseMoveEvent(QMouseEvent *event)
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

void checkPwdWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return  || event->key() == Qt::Key_Enter) 
    {
        ui->btn->click();
    }
    QWidget::keyPressEvent(event);
}

void checkPwdWidget::onSendCheckResult(bool res)
{
    ui->btn->setEnabled(true);
    if(!res)
    {
        ui->label_res->setText("密码验证失败，请重新验证");
        ui->lineEdit_pwd->clear();
    }
    else if(!ui->btn_return->isHidden())
    {
        emit reregisterCheckSuccessful();
    }
    else
    {
        emit checkSuccessful();
    }
}

void checkPwdWidget::onSendCheckOnlyResult(bool res)
{
    if(!res)
    {
        ui->label_res->setText("密码验证失败，请重新验证");
        ui->lineEdit_pwd->clear();
        ui->btn->setEnabled(true);
    }
    else
    {
        emit changeWidget();
    }
}

void checkPwdWidget::on_btn_clicked()
{
    QString username = ui->lineEdit_account->text();
    QString pwd = ui->lineEdit_pwd->text();
    
    ui->btn->setEnabled(false);
    
    if(!ui->btn_return->isVisible())
    {
        emit checkUserAndPwd(username, pwd);
    }
    else
    {
        tmpUsername = username;
        tmpPwd = pwd;
        emit checkUserAndPwdOnly(username, pwd);     
    }
}

void checkPwdWidget::on_btn_return_clicked()
{
    emit returnHomePage();
}

void checkPwdWidget::onShowReturnBtn()
{
    ui->btn_return->setVisible(true);
    ui->lineEdit_pwd->clear();
    ui->label_res->clear();
    ui->btn->setEnabled(true);
}

void checkPwdWidget::onReregisterCheckPwd()
{
    emit saveUserAndPwd(tmpUsername, tmpPwd);
}

void checkPwdWidget::on_btn_min_clicked()
{
    QWidget::showMinimized();
}

void checkPwdWidget::on_btn_close_clicked()
{
    QWidget::close();
}
