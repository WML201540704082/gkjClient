#ifndef MYPWDLINEDIT_H
#define MYPWDLINEDIT_H

#include <QAction>
#include <QLineEdit>
#include <QToolButton>

class myPwdLinEdit : public QLineEdit
{
    Q_OBJECT
public:
    myPwdLinEdit(QWidget *parent = nullptr);
    
private slots:
    void onPressed();
    void onReleased();
    
protected:
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);
    void resizeEvent(QResizeEvent *event);
    
private:
    void updateButtons();
    QToolButton *m_eye = nullptr;
    QToolButton *m_clear = nullptr;
    bool m_hovered = false;
};

#endif // MYPWDLINEDIT_H
