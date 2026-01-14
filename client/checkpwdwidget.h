#ifndef CHECKPWDWIDGET_H
#define CHECKPWDWIDGET_H

#include <QWidget>

namespace Ui {
class checkPwdWidget;
}

class checkPwdWidget : public QWidget
{
    Q_OBJECT

public:
    explicit checkPwdWidget(QWidget *parent = nullptr);
    ~checkPwdWidget();

    void iniUI();
    void screenAdapt();
    
    void mousePressEvent(QMouseEvent* event) override;

    void mouseMoveEvent(QMouseEvent* event) override;
    
    void keyPressEvent(QKeyEvent *event) override;
    
public slots:
    ///
    /// \brief onSendCheckResult
    /// \param res
    ///
    void onSendCheckResult(bool res);
    
    ///
    /// \brief onSendCheckOnlyResult
    /// \param res
    ///
    void onSendCheckOnlyResult(bool res);
    
    void onShowReturnBtn();
    
    void onReregisterCheckPwd();

private slots:
    void on_btn_clicked();
       
    void on_btn_return_clicked();
    
    void on_btn_min_clicked();
    
    void on_btn_close_clicked();
    
signals:
    void checkUserAndPwd(QString username, QString pwd);
    void saveUserAndPwd(QString username, QString pwd);
    void checkUserAndPwdOnly(QString username, QString pwd);//只检查不存储
    void checkSuccessful();
    void changeWidget();
    void reregisterCheckSuccessful();
    void returnHomePage();
    
private:
    Ui::checkPwdWidget *ui;
    
    QPoint whereismouse;
    
    QString tmpUsername;
    QString tmpPwd;
};

#endif // CHECKPWDWIDGET_H
