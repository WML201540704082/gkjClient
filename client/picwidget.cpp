#include "picwidget.h"
#include "ui_picwidget.h"
#include "globalVariables.h"

#include <QScreen>

picWidget::picWidget(const QString& uuidStr, const QString& username, QPixmap pixmap, QWidget *parent) :
    QWidget(parent), 
    ui(new Ui::picWidget),
    tmpUsrUuid(uuidStr), 
    tmpUsername(username), 
    tmpUsrPixmap(pixmap)
{
    ui->setupUi(this);
    screenAdpater();
    
    tmpUsrPixmap.scaled(ui->pushButton->width(), ui->pushButton->height(), 
                        Qt::IgnoreAspectRatio, Qt::SmoothTransformation); 
    ui->pushButton->setIconSize(QSize(ui->pushButton->width(), ui->pushButton->height()));
    ui->pushButton->setIcon(tmpUsrPixmap);
    
    ui->label->setText(tmpUsername);
}

picWidget::~picWidget()
{
    if (!tmpUsrPixmap.isNull())
    {
        tmpUsrPixmap = QPixmap();  // 替换为一个空 pixmap，释放原始显存
    }
    delete ui;
}

void picWidget::screenAdpater()
{
    if(w_fx == 0 || h_fx ==0)
    {
        QScreen* pScreen = QGuiApplication::primaryScreen();
        QRect rect1 = pScreen->geometry();

        w_fx = (double)rect1.size().width() / 1920.0;
        h_fx = (double)rect1.size().height() / 1080.0;
    }
    
    this->setMinimumSize(130 * w_fx, 140 * h_fx);
    this->setMaximumSize(130 * w_fx, 140 * h_fx);
    
    ui->pushButton->setMinimumSize(130 * w_fx, 120 * h_fx);
    ui->pushButton->setMaximumSize(130 * w_fx, 120 * h_fx);
    
    ui->label->setMinimumSize(130 * w_fx, 19 * h_fx);
    ui->label->setMaximumSize(130 * w_fx, 19 * h_fx);
    
    ui->verticalLayout->setSpacing(1 * h_fx);
}

void picWidget::on_pushButton_clicked()
{
    if(curType == facemanage)
    {
        emit loadTmpUserDetail(tmpUsrUuid, tmpUsername, tmpUsrPixmap);
    }
    else if(curType == chooseFace)
    {
        emit sendTmpUserFaceInfo(tmpUsrUuid, tmpUsername, tmpUsrPixmap);
    }
}
