#include "facemanagewidget.h"
#include "ui_facemanagewidget.h"
#include "utf8.h"
#include "globalVariables.h"

#include <QMouseEvent>
#include <QScreen>
#include <QScrollBar>
#include <QDebug>
#include <QMessageBox>

faceManageWidget::faceManageWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::faceManageWidget)
{
    ui->setupUi(this);
    iniUi();
    screenAdpater();
    
    setupScrollArea();
//    for(int i = 0; i < 10; i++)
//    {
//        addPicWidgetToGrid();
//    }
}

faceManageWidget::~faceManageWidget()
{
    delete ui;
    clearAllPicWidgets();
    delete containerWidget;
    containerWidget = nullptr;
}

void faceManageWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        whereismouse = event->pos();
    }
}

void faceManageWidget::mouseMoveEvent(QMouseEvent *event)
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

void faceManageWidget::iniUi()
{
    //去掉边框
    setWindowFlag(Qt::FramelessWindowHint);
    //窗口半透明（因为背景图片采用外阴影，使用该选项才能去除窗口无色边框）
    setAttribute(Qt::WA_TranslucentBackground);
}

void faceManageWidget::screenAdpater()
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
    
    ui->frame->setMinimumSize(550 * w_fx, 50 * h_fx);
    ui->frame->setMaximumSize(550 * w_fx, 50 * h_fx);
    
    ui->horizontalLayout->setContentsMargins(9 * w_fx, 9 * h_fx, 9 * w_fx, 9 * h_fx);
    ui->horizontalLayout->setSpacing(9 * w_fx);
    
    ui->lineEdit->setMinimumSize(250 * w_fx, 30 * h_fx);
    ui->lineEdit->setMaximumSize(250 * w_fx, 30 * h_fx);
    font = ui->lineEdit->font();
    font.setPointSize(8 * w_fx);
    ui->lineEdit->setFont(font);
    
    ui->btn_search->setMinimumSize(110 * w_fx, 30 * h_fx);
    ui->btn_search->setMaximumSize(110 * w_fx, 30 * h_fx);
    font = ui->btn_search->font();
    font.setPointSize(8 * w_fx);
    ui->btn_search->setFont(font);
    
    ui->btn_face->setMinimumSize(110 * w_fx, 30 * h_fx);
    ui->btn_face->setMaximumSize(110 * w_fx, 30 * h_fx);
    font = ui->btn_face->font();
    font.setPointSize(8 * w_fx);
    ui->btn_face->setFont(font);
    
    ui->frame_2->setMinimumSize(550 * w_fx, 310 * h_fx);
    ui->frame_2->setMaximumSize(550 * w_fx, 310 * h_fx);
    
    ui->scrollArea->setMinimumSize(540 * w_fx, 295 * h_fx);
    ui->scrollArea->setMaximumSize(540 * w_fx, 295 * h_fx);
    
    ui->gridLayout_2->setContentsMargins(2 * w_fx, 5 * h_fx, 2 * w_fx, 5 * h_fx);
}

void faceManageWidget::setupScrollArea()
{   
    ui->scrollArea->verticalScrollBar()->setStyleSheet(
        "QScrollBar:vertical {"
        "    background-color: rgba(245, 245, 245, 1);"
        "    width: 10px;"
        "    margin: 2px;"
        "    border: none;"
        "    border-radius: 5px;"
        "}"
        "QScrollBar::handle:vertical {"
        "    background-color: rgba(200, 200, 200, 1);"
        "    min-height: 20px;"
        "    border-radius: 5px;"
        "    margin: 1px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "    background-color: rgba(170, 170, 170, 1);"
        "}"
        "QScrollArea QScrollBar::handle:vertical:pressed {"
        "    background-color: rgba(140, 140, 140, 1);"
        "}"
        "QScrollArea QScrollBar:horizontal {"
        "    background-color: rgba(245, 245, 245, 1);"
        "    height: 10px;"
        "    border: none;"
        "    border-radius: 5px;"
        "    margin: 2px;"
        "}"
        "QScrollArea QScrollBar::handle:horizontal {"
        "    background-color: rgba(200, 200, 200, 1);"
        "    border-radius: 5px;"
        "    min-width: 20px;"
        "    margin: 1px;"
        "}"
        "QScrollArea QScrollBar::handle:horizontal:hover {"
        "    background-color: rgba(170, 170, 170, 1);"
        "}"
        "QScrollArea QScrollBar::handle:horizontal:pressed {"
        "    background-color: rgba(140, 140, 140, 1);"
        "}"
        "QScrollArea QScrollBar::add-line:vertical,"
        "QScrollArea QScrollBar::sub-line:vertical,"
        "QScrollArea QScrollBar::add-line:horizontal,"
        "QScrollArea QScrollBar::sub-line:horizonta {"
        "    background: none;"
        "    border: none;"
        "}"
        "QScrollArea QScrollBar::add-page:vertical,"
        "    QScrollArea QScrollBar::sub-page:vertical,"
        "    QScrollArea QScrollBar::add-page:horizontal,"
        "    QScrollArea QScrollBar::sub-page:horizontal {"
        "    background: none;"
        "}"
    );
    
    containerWidget = new QWidget();
    containerWidget->setStyleSheet("border: 1px solid rgba(212, 229, 248, 1);"
                                   "background-color: rgb(255, 255, 255);"
                                   "border-radius: 4px;");
    layout = new QGridLayout(containerWidget);
    layout->setAlignment(Qt::AlignTop);
    layout->setContentsMargins(0,0,0,0);
    layout->setHorizontalSpacing(2 * w_fx); // 设置间距
    layout->setVerticalSpacing(2 * h_fx); // 设置间距
    
    ui->scrollArea->setWidget(containerWidget);
    ui->scrollArea->setWidgetResizable(true); // 重要：让内容可以调整大小
    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

void faceManageWidget::addPicWidgetToGrid(const QString &uuidStr, const QString &username, QPixmap pixmap)
{
    picWidget* pPicWidget = new picWidget(uuidStr, username, pixmap, containerWidget);
    pPicWidget->curType = facemanage;
    connect(pPicWidget, &picWidget::loadTmpUserDetail, this, &faceManageWidget::loadTmpUserDetail);
    
    layout->addWidget(pPicWidget, row, col);
    
    col++;
    if(col >= maxCols)
    {
        col = 0;
        row++;
    }
}

//void faceManageWidget::removePicWidget(picWidget *widget)
//{
//    layout->removeWidget(widget);
//    widget->deleteLater(); // 安全删除
//}

void faceManageWidget::clearAllPicWidgets()
{
//    QLayoutItem* item = nullptr;
//    while ((item = layout->takeAt(0)) != nullptr)
//    {
//        if (item->widget()) 
//        {
////            item->widget()->deleteLater();
//            QWidget *widget = item->widget();
//            delete widget;
//            widget = nullptr;
//        }
//        delete item;
//        item = nullptr;
//    }
//    row = 0;
//    col = 0;
    
    if (!layout) return;
    
    QLayoutItem* item = nullptr;
    while ((item = layout->takeAt(0)) != nullptr)
    {
        if (item->widget())
        {
            delete item->widget();  // 直接删除子 widget
        }
        delete item; // 删除 layoutItem
    }
    
    // 重置行列计数，避免下一次继续往后叠加
    row = 0;
    col = 0;
}

void faceManageWidget::onSendTmpUsrInfo(QList<std::tuple<QString, QString, QPixmap> > list)
{
    clearAllPicWidgets();
    if(list.isEmpty())
    {
        return;
    }
    for(const std::tuple<QString, QString, QPixmap>& tuple : list)
    {
        QString uuidStr = std::get<0>(tuple);
        QString username = std::get<1>(tuple);
        QPixmap pixmap = std::get<2>(tuple);
        
        addPicWidgetToGrid(uuidStr, username, pixmap);
    }
}

void faceManageWidget::on_btn_min_clicked()
{
    QWidget::showMinimized();
}

void faceManageWidget::on_btn_close_clicked()
{
    QWidget::close();
}

void faceManageWidget::on_btn_return_clicked()
{
    emit returnTmpAuthManagePage();
}

void faceManageWidget::on_btn_search_clicked()
{
    QString tmpUsername = ui->lineEdit->text();
    if(tmpUsername.isEmpty())
    {
        emit getAllTmpUsrPicInfo();
        return;
    }
    
    emit getTmpUsrPicInfo(tmpUsername);
}

void faceManageWidget::on_btn_face_clicked()
{
    emit tmpUsrFaceRegister();
}
