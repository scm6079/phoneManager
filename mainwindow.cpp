#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_phoneManager = new PhoneManager(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


/*
countdown
lastCallLabel
phoneNumber
ringerOff
ringerOn
untilLabel
*/
