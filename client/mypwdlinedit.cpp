#include "mypwdlinedit.h"

#include <QStyle>

myPwdLinEdit::myPwdLinEdit(QWidget *parent) : QLineEdit(parent)
{
    setEchoMode(QLineEdit::Password);
    setClearButtonEnabled(false); // 用自定义清除按钮，避免与内置按钮抢位
    
    // 眼睛按钮
    m_eye = new QToolButton(this);
    m_eye->setAutoRaise(true);
    m_eye->setCursor(Qt::PointingHandCursor);
    m_eye->setIcon(QIcon(":/images/images/invisible.png"));
    m_eye->hide();
    connect(m_eye, &QToolButton::pressed, this, &myPwdLinEdit::onPressed);
    connect(m_eye, &QToolButton::released, this, &myPwdLinEdit::onReleased);
    
    // 清除按钮（最右侧）
    m_clear = new QToolButton(this);
    m_clear->setAutoRaise(true);
    m_clear->setCursor(Qt::ArrowCursor);
    m_clear->setIcon(QIcon(":/images/images/delete.png"));
//    m_clear->setIcon(style()->standardIcon(QStyle::SP_LineEditClearButton));
    m_clear->hide();
    connect(m_clear, &QToolButton::clicked, this, &QLineEdit::clear);
    
    m_clear->setAutoRaise(false);
    m_eye->setAutoRaise(false);
    m_clear->setFocusPolicy(Qt::NoFocus);
    m_eye->setFocusPolicy(Qt::NoFocus);
    m_clear->setStyleSheet("QToolButton{border:none;background:transparent;padding:0;}");
    m_eye->setStyleSheet("QToolButton{border:none;background:transparent;padding:0;}");
    
    // 文本变化时，控制清除按钮显隐并更新布局
    connect(this, &QLineEdit::textChanged, this, [this](const QString &t){
        m_clear->setVisible(!t.isEmpty() && (hasFocus() || m_hovered));
        updateButtons();
    });
}

void myPwdLinEdit::onPressed()
{
    m_eye->setIcon(QIcon(":/images/images/visible.png"));
    setEchoMode(QLineEdit::Normal);
}

void myPwdLinEdit::onReleased()
{
    m_eye->setIcon(QIcon(":/images/images/invisible.png"));
    setEchoMode(QLineEdit::Password);
}

void myPwdLinEdit::enterEvent(QEvent *event)
{
    m_hovered = true;
    m_eye->show();
    if (!text().isEmpty()) m_clear->show();
    updateButtons();
    QLineEdit::enterEvent(event);
}

void myPwdLinEdit::leaveEvent(QEvent *event)
{
    m_hovered = false;
    m_eye->hide();
    m_clear->hide();
    updateButtons();
    QLineEdit::leaveEvent(event);
}

void myPwdLinEdit::focusInEvent(QFocusEvent *event)
{
    m_eye->show();
    if (!text().isEmpty()) m_clear->show();
    updateButtons();
    QLineEdit::focusInEvent(event);
}

void myPwdLinEdit::focusOutEvent(QFocusEvent *event)
{
    m_eye->hide();
    m_clear->hide();
    updateButtons();
    QLineEdit::focusOutEvent(event);
}

void myPwdLinEdit::resizeEvent(QResizeEvent *event)
{
    QLineEdit::resizeEvent(event);
    updateButtons();
}

void myPwdLinEdit::updateButtons()
{
    const int fw = style()->pixelMetric(QStyle::PM_DefaultFrameWidth, nullptr, this);
    const int spacing = 2;
    
    int side = height() - 2*fw;
    side = qMax(16, side);                         // 防止太小或负值
    const int iconSide = qMax(12, int(side * 0.6));
    
    for (auto b : {m_clear, m_eye}) {
        b->setFixedSize(side, side);               // 正方形按钮
        b->setIconSize(QSize(iconSide, iconSide));
        // 可选：圆形点击区域，彻底消除“椭圆”观感
        // b->setMask(QRegion(0, 0, side, side, QRegion::Ellipse));
    }
    
    int right = rect().right() - fw;
    int used = 0;
    
    if (m_clear->isVisible()) {
        m_clear->move(right - side, (height() - side)/2);
        right -= side + spacing;
        used  += side + spacing;
    }
    if (m_eye->isVisible()) {
        m_eye->move(right - side, (height() - side)/2);
        right -= side + spacing;
        used  += side + spacing;
    }
    
    if (used > 0) used -= spacing;                 // 末尾不多留一个间距
    setTextMargins(0, 0, used, 0);
    
//    const int fw = style()->pixelMetric(QStyle::PM_DefaultFrameWidth, nullptr, this);
//    const int spacing = 2;
//    int right = rect().right() - fw;
    
//    // 右起：清除按钮 -> 眼睛按钮
//    int used = 0;
//    if (m_clear->isVisible()) {
//        const QSize s = m_clear->sizeHint();
//        m_clear->move(right - s.width(), (height() - s.height())/2);
//        right -= s.width() + spacing;
//        used += s.width() + spacing;
//    }
//    if (m_eye->isVisible()) {
//        const QSize s = m_eye->sizeHint();
//        m_eye->move(right - s.width(), (height() - s.height())/2);
//        right -= s.width() + spacing;
//        used += s.width() + spacing;
//    }
    
//    setTextMargins(0, 0, used, 0); // 右侧为按钮预留空间，避免文本被遮挡
}
