#ifndef DETAILWIDGET_H
#define DETAILWIDGET_H

#include <QWidget>

namespace Ui {
class detailWidget;
}

class detailWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit detailWidget(QWidget *parent = nullptr);
    ~detailWidget();
    
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
    /// \brief iniTableWidget 初始化tableWidget
    ///
    void iniTableWidget();
    
    ///
    /// \brief loadTmpUserDetailInfo 显示临时用户详细信息
    /// \param uuidStr
    /// \param username
    /// \param pixmap
    ///
    void loadTmpUserDetailInfo(const QString &uuidStr, const QString &username, QPixmap pixmap);
    
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
    
    ///
    /// \brief reloadUserInfo 重新加载临时用户人脸信息
    ///
    void reloadUserInfo();
    
    ///
    /// \brief reloadUserPeriod 重新加载临时用户时间信息
    ///
    void reloadUserPeriod();
    
    ///
    /// \brief updateTable 更新tableWidget
    ///
    void updateTable();
    
public slots:
    ///
    /// \brief onSendTmpUserPeriodsData
    /// \param uuidStr
    /// \param dataList
    ///
    void onSendTmpUserPeriodsData(const QString& uuidStr, QList<QPair<QString, QString> > dataList);
    
    ///
    /// \brief onSendDeleteTmpUserRes
    /// \param res
    ///
    void onSendDeleteTmpUserRes(bool res);
    
    ///
    /// \brief onSendFacePixmap
    /// \param pixmap
    ///
    void onSendFacePixmap(QPixmap pixmap);
    
    ///
    /// \brief onSendDelTmpUserDataResult2
    /// \param result
    /// \param index
    ///
    void onSendDelTmpUserDataResult2(bool result, int index);
    
private slots:
    void on_btn_min_clicked();
    
    void on_btn_close_clicked();
    
    void on_btn_return_clicked();
    
    void on_btn_delete_clicked();
    
    void on_btn_change_clicked();
    
    void on_btn_add_clicked();
    
signals:
    void returnFaceManagePage();
    void getTmpUserData(const QString& uuidStr);
    void deleteTmpUser(const QString& uuidStr);
    void changeFacialInfo(const QString& uuidStr, const QString& tmpUsername);
    void getFacePixmap(const QString& tmpUserUuid);
    void delTmpUserEncFile(QString username);
    void addTmpUserPeriod(const QString& uuidStr, const QString& tmpUsername, QPixmap tmpPixmap);
    void deleteTmpUserData(int index, const QString& uuidStr, const QString& startTime, const QString& endTime);
    void changeTmpUserPeriod(const QString& uuidStr, const QString& tmpUsername, QPixmap tmpPixmap,
                            const QString& startTime, const QString& endTime);
    void startSync();
    
private:
    Ui::detailWidget *ui;
    
    QString tmpUserUuid;
    QString tmpUsername;
    QPixmap tmpPixmap;
    
    QList<QPair<QString, QString> > allData;
    
    QPoint whereismouse;
};

#endif // DETAILWIDGET_H
