#include "tmpauthmanagewidget.h"
#include "ui_tmpauthmanagewidget.h"
#include "utf8.h"
#include "globalVariables.h"

#include <QHBoxLayout>
#include <QMouseEvent>
#include <QScreen>
#include <QSettings>
#include <QFile>
#include <QDebug>
#include <QMessageBox>

tmpAuthManageWidget::tmpAuthManageWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::tmpAuthManageWidget)
{
    ui->setupUi(this);
    iniUi();  
    iniCheckBox();
    screenAdpater(); 
    iniTableWidget();
}

tmpAuthManageWidget::~tmpAuthManageWidget()
{
    delete ui;
}

void tmpAuthManageWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        whereismouse = event->pos();
    }
}

void tmpAuthManageWidget::mouseMoveEvent(QMouseEvent *event)
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

void tmpAuthManageWidget::iniUi()
{
    //去掉边框
    setWindowFlag(Qt::FramelessWindowHint);
    //窗口半透明（因为背景图片采用外阴影，使用该选项才能去除窗口无色边框）
    setAttribute(Qt::WA_TranslucentBackground);
}

void tmpAuthManageWidget::screenAdpater()
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
    
    ui->checkBox->setMinimumSize(250 * w_fx, 30 * h_fx);
    ui->checkBox->setMaximumSize(250 * w_fx, 30 * h_fx);
    font = ui->checkBox->font();
    font.setPointSize(8 * w_fx);
    ui->checkBox->setFont(font);
    
    ui->pushButton->setMinimumSize(110 * w_fx, 30 * h_fx);
    ui->pushButton->setMaximumSize(110 * w_fx, 30 * h_fx);
    font = ui->pushButton->font();
    font.setPointSize(8 * w_fx);
    ui->pushButton->setFont(font);
    
    ui->pushButton_2->setMinimumSize(110 * w_fx, 30 * h_fx);
    ui->pushButton_2->setMaximumSize(110 * w_fx, 30 * h_fx);
    font = ui->pushButton_2->font();
    font.setPointSize(8 * w_fx);
    ui->pushButton_2->setFont(font);
    
    ui->frame_3->setMinimumSize(550 * w_fx, 310 * h_fx);
    ui->frame_3->setMaximumSize(550 * w_fx, 310 * h_fx);
    
    ui->verticalLayout_3->setContentsMargins(7 * w_fx, 0, 7 * w_fx, 9 * h_fx);
    ui->verticalLayout_3->setSpacing(6 * h_fx);
    
    ui->frame_2->setMinimumSize(535 * w_fx, 290 * h_fx);
    ui->frame_2->setMaximumSize(535 * w_fx, 290 * h_fx);
    
    ui->verticalLayout_2->setContentsMargins(5 * w_fx, 5 * h_fx, 5 * w_fx, 5 * h_fx);
    ui->verticalLayout_2->setSpacing(5 * h_fx);
    
    ui->tableWidget->setMinimumSize(525 * w_fx, 245 * h_fx);
    ui->tableWidget->setMaximumSize(525 * w_fx, 245 * h_fx);
    
    ui->widget_3->setMinimumSize(525 * w_fx, 30 * h_fx);
    ui->widget_3->setMaximumSize(525 * w_fx, 30 * h_fx);
    
    ui->horizontalLayout_2->setContentsMargins(9 * w_fx, 0, 9 * w_fx, 0);
    ui->horizontalLayout_2->setSpacing(6 * w_fx);
    
    ui->label->setMinimumSize(130 * w_fx, 30 * h_fx);
    ui->label->setMaximumSize(130 * w_fx, 30 * h_fx);
    font = ui->label->font();
    font.setPointSize(8 * w_fx);
    ui->label->setFont(font);
    
    ui->btn_pre->setMinimumSize(100 * w_fx, 30 * h_fx);
    ui->btn_pre->setMaximumSize(100 * w_fx, 30 * h_fx);
    font = ui->btn_pre->font();
    font.setPointSize(8 * w_fx);
    ui->btn_pre->setFont(font);
    
    ui->btn_next->setMinimumSize(100 * w_fx, 30 * h_fx);
    ui->btn_next->setMaximumSize(100 * w_fx, 30 * h_fx);
    font = ui->btn_next->font();
    font.setPointSize(8 * w_fx);
    ui->btn_next->setFont(font);
}

void tmpAuthManageWidget::iniCheckBox()
{
    emit getTempUserLogin();
}

void tmpAuthManageWidget::iniTableWidget()
{
    rowsPerPage = 7;
    ui->tableWidget->setColumnCount(5);
    QStringList headers = {"序号", "使用人", "开始时间", "结束时间", "操作"};
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableWidget->setAlternatingRowColors(true);
    
    ui->tableWidget->setColumnWidth(0, 50 * w_fx);  // 序号列宽
    ui->tableWidget->setColumnWidth(1, 90 * w_fx); // 用户名列宽
    ui->tableWidget->setColumnWidth(2, 130 * w_fx); // 开始时间列宽
    ui->tableWidget->setColumnWidth(3, 130 * w_fx); // 结束时间列宽
    ui->tableWidget->setColumnWidth(4, 125 * w_fx); // 操作列宽
    
    QFont font("微软雅黑");
    font.setPointSizeF(6 * w_fx);
    ui->tableWidget->setFont(font);
    ui->tableWidget->horizontalHeader()->setFont(font);
 
//    emit getAllTmpUserData();
}

void tmpAuthManageWidget::updateTable()
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0); 
    
    //确保 QHeaderView 不会自动调整列宽，确保可以调整表格列宽
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);    
    
    int start = currentPage * rowsPerPage;
    int end = qMin(start + rowsPerPage, allData.size());
    
    for(int i = start; i < end; ++i)
    {
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        
        // 插入数据
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(i + 1)));  // 序号
        ui->tableWidget->item(row, 0)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->item(row, 0)->setFlags(
                    ui->tableWidget->item(row, 0)->flags() & ~Qt::ItemIsEditable);
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(allData[i].first,Qt::AlignHCenter));  // 用户名
        ui->tableWidget->item(row, 1)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->item(row, 1)->setFlags(
                    ui->tableWidget->item(row, 1)->flags() & ~Qt::ItemIsEditable);
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(allData[i].second.first));  // 开始时间
        ui->tableWidget->item(row, 2)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->item(row, 2)->setFlags(
                    ui->tableWidget->item(row, 2)->flags() & ~Qt::ItemIsEditable);
        ui->tableWidget->setItem(row, 3, new QTableWidgetItem(allData[i].second.second)); // 结束时间
        ui->tableWidget->item(row, 3)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->item(row, 3)->setFlags(
                    ui->tableWidget->item(row, 3)->flags() & ~Qt::ItemIsEditable);
        
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
        
        ui->tableWidget->setCellWidget(row, 4, widget);
    }
    
    // 确保 setColumnWidth() 生效，确保可以调整表格列宽
    ui->tableWidget->setColumnWidth(0, 50 * w_fx);  // 序号列宽
    ui->tableWidget->setColumnWidth(1, 90 * w_fx); // 用户名列宽
    ui->tableWidget->setColumnWidth(2, 130 * w_fx); // 开始时间列宽
    ui->tableWidget->setColumnWidth(3, 130 * w_fx); // 结束时间列宽
    ui->tableWidget->setColumnWidth(4, 125 * w_fx); // 操作列宽
    
    // 手动触发界面更新，确保可以调整表格列宽
    ui->tableWidget->update();
    ui->tableWidget->repaint();
}

void tmpAuthManageWidget::editRow(int index)
{
    QString tmpUserName = allData[index].first;
    QString startTime = allData[index].second.first;
    QString endTime = allData[index].second.second;
    emit changeTmpUserPeriod(tmpUserName, startTime, endTime);
}

void tmpAuthManageWidget::deleteRow(int index)
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(nullptr, "删除确认", "确定删除该用户的记录吗？", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) 
    {
        QString tmpUserName = allData[index].first;
        QString startTime = allData[index].second.first;
        QString endTime = allData[index].second.second;
        emit deleteTmpUserData(index, tmpUserName, startTime, endTime);
    }
}

void tmpAuthManageWidget::onSendAllTmpUserData(QList<QPair<QString, QPair<QString, QString> > > dataList)
{
    allData = dataList;
    int count = allData.size();
    ui->label->setText(QString("共%1条").arg(count));
    updateTable();    
}

void tmpAuthManageWidget::onSendTmpUserLogin(bool tmpUserLogin)
{
    ui->checkBox->setCheckState(tmpUserLogin ? Qt::Checked : Qt::Unchecked);
}

void tmpAuthManageWidget::onSendDelTmpUserDataResult(bool result, int index)
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

void tmpAuthManageWidget::on_btn_next_clicked()
{
    if ((currentPage + 1) * rowsPerPage < allData.size())
    {
        currentPage++;
        updateTable();
    }
}

void tmpAuthManageWidget::on_btn_pre_clicked()
{
    if (currentPage > 0)
    {
        currentPage--;
        updateTable();
    }
}

void tmpAuthManageWidget::on_pushButton_clicked()
{
    emit authorize();
}

void tmpAuthManageWidget::on_btn_return_clicked()
{
    emit returnHomePage();
}

void tmpAuthManageWidget::on_btn_min_clicked()
{
    QWidget::showMinimized();
}

void tmpAuthManageWidget::on_btn_close_clicked()
{
    QWidget::close();
}

void tmpAuthManageWidget::on_checkBox_stateChanged(int arg1)
{
    if(arg1 == 0)
    {
        emit setTempUserLogin(false);
    }
    else if(arg1 == 2)
    {
        emit setTempUserLogin(true);
    }
}

void tmpAuthManageWidget::on_pushButton_2_clicked()
{
    emit faceManage();
}
