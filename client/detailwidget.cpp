#include "detailwidget.h"
#include "ui_detailwidget.h"
#include "utf8.h"
#include "globalVariables.h"

#include <QScreen>
#include <QMouseEvent>
#include <QMessageBox>
#include <QDebug>

detailWidget::detailWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::detailWidget)
{
    ui->setupUi(this);
    iniUi();
    screenAdpater();
    iniTableWidget();
}

detailWidget::~detailWidget()
{
    delete ui;
}

void detailWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        whereismouse = event->pos();
    }
}

void detailWidget::mouseMoveEvent(QMouseEvent *event)
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

void detailWidget::iniUi()
{
    //去掉边框
    setWindowFlag(Qt::FramelessWindowHint);
    //窗口半透明（因为背景图片采用外阴影，使用该选项才能去除窗口无色边框）
    setAttribute(Qt::WA_TranslucentBackground);
}

void detailWidget::screenAdpater()
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
    
    ui->gridLayout_2->setContentsMargins(9 * w_fx, 9 * h_fx, 9 * w_fx, 9 * h_fx);
    ui->gridLayout_2->setVerticalSpacing(6 * h_fx);
    ui->gridLayout_2->setHorizontalSpacing(6 * w_fx);
    
    ui->verticalLayout_2->setContentsMargins(5 * w_fx, 5 * h_fx, 5 * w_fx, 5 * h_fx);
    ui->verticalLayout_2->setSpacing(5 * h_fx);
    
    ui->frame_2->setMinimumSize(518 * w_fx, 120 * h_fx);
    ui->frame_2->setMaximumSize(518 * w_fx, 120 * h_fx);
    
    ui->label_face->setMaximumSize(130 * w_fx, 120 * h_fx);
    ui->label_face->setMinimumSize(130 * w_fx, 120 * h_fx);
    
    ui->frame_4->setMinimumSize(386 * w_fx, 120 * h_fx);
    ui->frame_4->setMaximumSize(386 * w_fx, 120 * h_fx);
    
    ui->gridLayout_3->setContentsMargins(20 * w_fx, 9 * h_fx, 20 * w_fx, 9 * h_fx);
    ui->gridLayout_3->setVerticalSpacing(6 * h_fx);
    ui->gridLayout_3->setHorizontalSpacing(23 * w_fx);
    
    ui->label_name->setMinimumSize(346 * w_fx, 30 * h_fx);
    ui->label_name->setMaximumSize(346 * w_fx, 30 * h_fx);
    font = ui->label_name->font();
    font.setPointSize(8 * w_fx);
    ui->label_name->setFont(font);
    
    ui->btn_change->setMinimumSize(100 * w_fx, 30 * h_fx);
    ui->btn_change->setMaximumSize(100 * w_fx, 30 * h_fx);
    font = ui->btn_change->font();
    font.setPointSize(8 * w_fx);
    ui->btn_change->setFont(font);
    
    ui->btn_delete->setMinimumSize(100 * w_fx, 30 * h_fx);
    ui->btn_delete->setMaximumSize(100 * w_fx, 30 * h_fx);
    font = ui->btn_delete->font();
    font.setPointSize(8 * w_fx);
    ui->btn_delete->setFont(font);
    
    ui->btn_add->setMinimumSize(100 * w_fx, 30 * h_fx);
    ui->btn_add->setMaximumSize(100 * w_fx, 30 * h_fx);
    font = ui->btn_add->font();
    font.setPointSize(8 * w_fx);
    ui->btn_add->setFont(font);
}

void detailWidget::iniTableWidget()
{
    ui->tableWidget->setColumnCount(4);
    QStringList headers = {"序号", "开始时间", "结束时间", "操作"};
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableWidget->setAlternatingRowColors(true);
    
    ui->tableWidget->setColumnWidth(0, 50 * w_fx);  // 序号列宽
    ui->tableWidget->setColumnWidth(1, 170 * w_fx); // 用户名列宽
    ui->tableWidget->setColumnWidth(2, 170 * w_fx); // 开始时间列宽
    ui->tableWidget->setColumnWidth(3, 130 * w_fx); // 结束时间列宽
    
    QFont font("微软雅黑", 7 * w_fx);
    ui->tableWidget->setFont(font);
    ui->tableWidget->horizontalHeader()->setFont(font);
}

void detailWidget::loadTmpUserDetailInfo(const QString &uuidStr, const QString &username, QPixmap pixmap)
{
    tmpUserUuid = uuidStr;
    tmpUsername = username;
    tmpPixmap = pixmap.scaled(ui->label_face->width(), ui->label_face->height(), 
                        Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    
    ui->label_face->setPixmap(tmpPixmap);
    ui->label_face->setScaledContents(true);

    ui->label_name->setText(QString("姓名：%1").arg(username));
    
    emit getTmpUserData(tmpUserUuid);
}

void detailWidget::editRow(int index)
{
    QString startTime = allData[index].first;
    QString endTime = allData[index].second;
    emit changeTmpUserPeriod(tmpUserUuid, tmpUsername, tmpPixmap, startTime, endTime);
}

void detailWidget::deleteRow(int index)
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(nullptr, "删除确认", "确定删除该用户的记录吗？", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) 
    {
        QString startTime = allData[index].first;
        QString endTime = allData[index].second;
        emit deleteTmpUserData(index, tmpUserUuid, startTime, endTime);
    }
}

void detailWidget::reloadUserInfo()
{
    emit getFacePixmap(tmpUserUuid);
}

void detailWidget::reloadUserPeriod()
{
    emit getTmpUserData(tmpUserUuid);
}

void detailWidget::updateTable()
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0); 
    
    //确保 QHeaderView 不会自动调整列宽，确保可以调整表格列宽
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed); 
    
    for(int i = 0; i < allData.size(); ++i)
    {
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        
        // 插入数据
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(i + 1)));  // 序号
        ui->tableWidget->item(row, 0)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->item(row, 0)->setFlags(
                    ui->tableWidget->item(row, 0)->flags() & ~Qt::ItemIsEditable);
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(allData[i].first, Qt::AlignHCenter));  // 开始时间
        ui->tableWidget->item(row, 1)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->item(row, 1)->setFlags(
                    ui->tableWidget->item(row, 1)->flags() & ~Qt::ItemIsEditable);
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(allData[i].second, Qt::AlignHCenter));  // 结束时间
        ui->tableWidget->item(row, 2)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->item(row, 2)->setFlags(
                    ui->tableWidget->item(row, 2)->flags() & ~Qt::ItemIsEditable);
        // 添加修改和删除按钮
        QPushButton* editBtn = new QPushButton("修改");
        QPushButton* delBtn = new QPushButton("删除");
        
        // 设置按钮的固定大小，避免列被撑开，确保可以调整表格列宽
        editBtn->setFixedSize(50 * w_fx, 25 * h_fx);
        delBtn->setFixedSize(50 * w_fx, 25 * h_fx);
        // 限制按钮不自动扩展，确保可以调整表格列宽
        editBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        delBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        
        connect(editBtn, &QPushButton::clicked, [=]() { editRow(i); });
        connect(delBtn, &QPushButton::clicked, [=]() { deleteRow(i); });
        
        QWidget* widget = new QWidget();
        QHBoxLayout* layout = new QHBoxLayout(widget);
        layout->addWidget(editBtn);
        layout->addWidget(delBtn);
        layout->setContentsMargins(0, 0, 0, 0);
        widget->setLayout(layout);
        
        // 设置固定大小，防止 cellWidget 撑开列宽，确保可以调整表格列宽
        widget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        
        ui->tableWidget->setCellWidget(row, 3, widget);
    }
}

void detailWidget::onSendTmpUserPeriodsData(const QString &uuidStr, QList<QPair<QString, QString> > dataList)
{
    allData = dataList;
    
    if(tmpUserUuid != uuidStr)
    {
        return;
    }
    
    updateTable();
}

void detailWidget::onSendDeleteTmpUserRes(bool res)
{
    if(res)
    {
        emit startSync();
        emit returnFaceManagePage();
    }
    else
    {
        QMessageBox::warning(NULL, "删除失败", "删除失败，请重试！");
    }
}

void detailWidget::onSendFacePixmap(QPixmap pixmap)
{
    qDebug()<<"onSendFacePixmap(QPixmap pixmap)";
    QPixmap tmpPixmap = pixmap.scaled(ui->label_face->width(), ui->label_face->height(), 
                        Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    
    ui->label_face->setPixmap(tmpPixmap);
    ui->label_face->setScaledContents(true);
}

void detailWidget::onSendDelTmpUserDataResult2(bool result, int index)
{
    if(!result)
    {
        QMessageBox::warning(nullptr, "错误", "删除临时用户数据失败，请重试");
    }
    else
    {
        emit startSync();
        allData.removeAt(index);
        updateTable();
    }
}

void detailWidget::on_btn_min_clicked()
{
    QWidget::showMinimized();
}

void detailWidget::on_btn_close_clicked()
{
    QWidget::close();
}

void detailWidget::on_btn_return_clicked()
{
    returnFaceManagePage();
}

void detailWidget::on_btn_delete_clicked()
{
    QMessageBox:: StandardButton result = QMessageBox::information
            (NULL, "删除用户", "确定删除该临时用户？", QMessageBox::Yes|QMessageBox::No);
    switch (result)
    {
    case QMessageBox::Yes:
    {
        emit deleteTmpUser(tmpUserUuid);
        emit delTmpUserEncFile(tmpUsername);
    }
    case QMessageBox::No:
        return;
    default:
        break;
    }
}

void detailWidget::on_btn_change_clicked()
{
    emit changeFacialInfo(tmpUserUuid, tmpUsername);
}

void detailWidget::on_btn_add_clicked()
{
    emit addTmpUserPeriod(tmpUserUuid, tmpUsername, tmpPixmap);
}
