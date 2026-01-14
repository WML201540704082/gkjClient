#ifndef TMPAUTHMANAGEWIDGET_H
#define TMPAUTHMANAGEWIDGET_H

#include <QWidget>

#include "utf8.h"

namespace Ui {
class tmpAuthManageWidget;
}

class tmpAuthManageWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit tmpAuthManageWidget(QWidget *parent = nullptr);
    ~tmpAuthManageWidget();
    
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
    /// \brief iniCheckBox
    ///
    void iniCheckBox();
    
    ///
    /// \brief iniTableWidget 初始化tableWidget
    ///
    void iniTableWidget();
    
    ///
    /// \brief updateTable 更新表格
    ///
    void updateTable();
    
    ///
    /// \brief editRow 编辑tableWidget某行数据
    /// \param index 
    ///
    void editRow(int index);
    
    ///
    /// \brief deleteRow 删除tableWidget某行数据
    /// \param index
    ///
    void deleteRow(int index);
    
public slots:
    void onSendAllTmpUserData(QList<QPair<QString, QPair<QString, QString> > > dataList);
    void onSendTmpUserLogin(bool tmpUserLogin);
    void onSendDelTmpUserDataResult(bool result, int index);
    
signals:
    void getAllTmpUserData();
    void returnHomePage();
    void authorize();
    void getTempUserLogin();
    void setTempUserLogin(bool tmpUserLogin);
    void deleteTmpUserData(int index, QString tmpUserName, QString startTime, QString endTime);
    void faceManage();
    void changeTmpUserPeriod(const QString &tmpUserName,const QString &startTime, const QString &endTime);
    void startSync();
    
private slots:
    void on_btn_next_clicked();
    
    void on_btn_pre_clicked();
    
    void on_pushButton_clicked();
    
    void on_btn_return_clicked();
    
    void on_btn_min_clicked();
    
    void on_btn_close_clicked();
    
    void on_checkBox_stateChanged(int arg1);
    
    void on_pushButton_2_clicked();
    
private:
    Ui::tmpAuthManageWidget *ui;
    
    QPoint whereismouse;
    
    QList<QPair<QString, QPair<QString, QString> > > allData;
    int currentPage = 0;
    int rowsPerPage = 0;
};

#endif // TMPAUTHMANAGEWIDGET_H
