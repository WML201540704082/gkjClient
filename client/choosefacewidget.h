#ifndef CHOOSEFACEWIDGET_H
#define CHOOSEFACEWIDGET_H

#include <QWidget>
#include <QGridLayout>

#include "picwidget.h"

namespace Ui {
class chooseFaceWidget;
}

class chooseFaceWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit chooseFaceWidget(QWidget *parent = nullptr);
    ~chooseFaceWidget();
    
    ///
    /// \brief mousePressEvent 重写mousePressEvent和mouseMoveEvent用于实现程序拖动及设置可拖动控件区域
    /// \param event
    ///
    void mousePressEvent(QMouseEvent* event) override;

    ///
    /// \brief mouseMoveEvent 重写mousePressEvent和mouseMoveEvent用于实现程序拖动及设置可拖动控件区域
    /// \param event
    ///
    void mouseMoveEvent(QMouseEvent* event) override;
    
    ///
    /// \brief iniUi 初始化UI界面
    ///
    void iniUi();
    
    ///
    /// \brief screenAdpater 适配屏幕分辨率
    ///
    void screenAdpater();
    
    ///
    /// \brief setupScrollArea 设置scrollArea
    ///
    void setupScrollArea();
    
    ///
    /// \brief addPicWidgetToGrid 将picwidget添加到scrollArea
    /// \param uuidStr
    /// \param username
    /// \param pixmap
    ///
    void addPicWidgetToGrid(const QString& uuidStr, const QString& username, QPixmap pixmap);
    
//    ///
//    /// \brief removePicWidget 移除picwidget
//    /// \param widget
//    ///
//    void removePicWidget(picWidget* widget);
    
    ///
    /// \brief clearAllPicWidgets 删除scrollArea中的所有picwidget
    ///
    void clearAllPicWidgets();
    
public slots:
    ///
    /// \brief onSendTmpUsrFaceInfo 
    /// \param list
    ///
    void onSendTmpUsrFaceInfo(QList<std::tuple<QString, QString, QPixmap>> list);
    
signals:
    void getAllTmpUsrFaceInfo();
    void sendTmpUserFaceInfo(const QString& uuidStr, const QString& username, QPixmap pixmap);
    void returnTmpAuthPage();
    void getTmpUsrFaceInfo(QString inputUsername);
    
private slots:
    void on_btn_return_clicked();
    
    void on_btn_min_clicked();
    
    void on_btn_close_clicked();
    
    void on_btn_search_clicked();
    
private:
    Ui::chooseFaceWidget *ui;

    QPoint whereismouse;
    
    QWidget* containerWidget = nullptr;
    QGridLayout* layout = nullptr;
    
    int row = 0, col = 0;
    const int maxCols = 4; // 每行最多3个
};

#endif // CHOOSEFACEWIDGET_H
