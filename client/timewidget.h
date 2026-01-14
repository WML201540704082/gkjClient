#ifndef TIMEWIDGET_H
#define TIMEWIDGET_H

#include <QWidget>

namespace Ui {
class timeWidget;
}

enum timeType
{
    add = 1,
    change1 = 2,
    change2 = 3
};

class timeWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit timeWidget(QWidget *parent = nullptr);
    ~timeWidget();
    
    timeType curType;
    QString curUuid;
    QString curUsername;
    
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
    /// \brief setUsername 设置lineEdit用户名
    /// \param username
    ///
    void setUsername(QString username);
    
    ///
    /// \brief setPixmap
    /// \param tmpPixmap
    ///
    void setPixmap(QPixmap tmpPixmap);
    
    ///
    /// \brief setDateTime
    /// \param startTime
    /// \param endTime
    ///
    void setDateTime(QString startTime, QString endTime);
    
    ///
    /// \brief setDateTime
    ///
    void setDateTime();
    
    ///
    /// \brief showFloatingErrorWindow 显示错误信息悬浮窗
    /// \param str
    ///
    void showFloatingErrorWindow(QString str);
    
public slots:
    ///
    /// \brief onSendAddUserPeriodRes 处理添加时间段结果槽函数
    /// \param result
    ///
    void onSendAddUserPeriodRes(bool result);
    
    ///
    /// \brief onSendChangeUserPeriodRes 处理更新时间段结果槽函数
    /// \param result
    ///
    void onSendChangeUserPeriodRes(bool result);
    
private slots:
    void on_btn_min_clicked();
    
    void on_btn_close_clicked();
    
    void on_btn_return_clicked();
    
    void on_pushButton_3_clicked();
    
signals:
    void returnDetailPage();
    void addUserPeriod(const QString &uuid, const QString &startTime, const QString &endTime);
    void addUserPeriodSuccess();
    void changeUserPeriod(const QString &uuid, const QString &oldStartTime, const QString &oldEndTime, const QString &startTime, const QString &endTime);
    void changeUserPeriodSuccess();
    void changeUserPeriodSuccess2();
    void returnAuthManagePage();
    void startSync();
    
private:
    Ui::timeWidget *ui;
    
    QPoint whereismouse;
    
    QString oldStartTime;
    QString oldEndTime;
};

#endif // TIMEWIDGET_H
