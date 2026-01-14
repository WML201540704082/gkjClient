#include "floatingerrorwindow.h"
#include "ui_floatingerrorwindow.h"

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QTimer>

#include <QPixmap>

floatingErrorWindow::floatingErrorWindow(const QString &message,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::floatingErrorWindow)
{
    // 设置窗口的大小
    setFixedSize(400, 100);
    
    // 去掉边框
    setWindowFlag(Qt::FramelessWindowHint);  
    
    // 设置背景透明
    setAttribute(Qt::WA_TranslucentBackground);  
    
    // close时自动析构
    setAttribute(Qt::WA_DeleteOnClose);
        
    // 创建一个标签用于显示错误信息
    QLabel* label = new QLabel(message, this);
    label->setStyleSheet("background-color: rgba(255, 252, 233, 180); border-radius: 10px; color: rgb(255, 75, 44);");
    label->setAlignment(Qt::AlignCenter);
    label->setText(tr("<html><img src='%1' width='20' height='20' style='vertical-align: bottom;'> %2 </html>").arg(":/images/images/warning.png").arg(message));
    
    // 设置布局
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(label);
    
    
    // 设置定时器，1.5秒后关闭窗口
    QTimer::singleShot(1000, this, &QWidget::close);
}

floatingErrorWindow::~floatingErrorWindow()
{
    delete ui;
}
