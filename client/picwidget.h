#ifndef PICWIDGET_H
#define PICWIDGET_H

#include <QWidget>

namespace Ui {
class picWidget;
}

enum clickType
{
    facemanage = 1,
    chooseFace = 2
};

class picWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit picWidget(const QString& uuidStr, const QString& username, QPixmap pixmap, QWidget *parent = nullptr);
    ~picWidget();
    
    ///
    /// \brief screenAdpater 适配屏幕分辨率
    ///
    void screenAdpater();
    
    clickType curType;
    
private slots:
    void on_pushButton_clicked();
    
signals:
    void loadTmpUserDetail(const QString& uuidStr, const QString& username, QPixmap pixmap);
    void sendTmpUserFaceInfo(const QString& uuidStr, const QString& username, QPixmap pixmap);
    
private:
    Ui::picWidget *ui;
    
    QString tmpUsrUuid;
    QString tmpUsername;
    QPixmap tmpUsrPixmap;
};

#endif // PICWIDGET_H
