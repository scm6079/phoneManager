#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class PhoneManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void setRinger(bool);
    void setUntil(QString);
    void setCountdown(QString);
    void setLastCall(QString, QString);

protected:


private:
    Ui::MainWindow *ui;
    PhoneManager *m_phoneManager;
};

#endif // MAINWINDOW_H
