#ifndef FACEMANAGEWIDGET_H
#define FACEMANAGEWIDGET_H

#include "picwidget.h"

#include <QWidget>
#include <QGridLayout>

namespace Ui {
class faceManageWidget;
}

class faceManageWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit faceManageWidget(QWidget *parent = nullptr);
    ~faceManageWidget();
    
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
    void onSendTmpUsrInfo(QList<std::tuple<QString, QString, QPixmap>> list);
    
private slots:
    void on_btn_min_clicked();
    
    void on_btn_close_clicked();
    
    void on_btn_return_clicked();
    
    void on_btn_search_clicked();
    
    void on_btn_face_clicked();
    
signals:
    void returnTmpAuthManagePage();
    void getAllTmpUsrPicInfo();
    void getTmpUsrPicInfo(QString tmpUsername);
    void tmpUsrFaceRegister();
    void loadTmpUserDetail(const QString& uuidStr, const QString& username, QPixmap pixmap);
    
private:
    Ui::faceManageWidget *ui;
    
    QPoint whereismouse;
    
    QWidget* containerWidget = nullptr;
    QGridLayout* layout = nullptr;
    
    int row = 0, col = 0;
    const int maxCols = 4; // 每行最多3个
};

#endif // FACEMANAGEWIDGET_H
