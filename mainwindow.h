#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "phoneManager.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:


private:
    Ui::MainWindow *ui;
    PhoneManager *m_phoneManager;
};

#endif // MAINWINDOW_H
