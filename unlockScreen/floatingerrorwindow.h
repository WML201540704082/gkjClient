#ifndef FLOATINGERRORWINDOW_H
#define FLOATINGERRORWINDOW_H

#include <QWidget>

namespace Ui {
class floatingErrorWindow;
}

class floatingErrorWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit floatingErrorWindow(const QString& message, QWidget *parent = nullptr);
    ~floatingErrorWindow();
    
private:
    Ui::floatingErrorWindow *ui;
};

#endif // FLOATINGERRORWINDOW_H
