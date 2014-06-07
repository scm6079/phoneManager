#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "phonemanager.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setRinger(true);
    setUntil("");
    setCountdown("");
    setLastCall("", "", "");

    m_phoneManager = new PhoneManager(this, this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setRinger(bool bOn)
{
    qDebug("set Ringer!");
    if(bOn) {
        ui->lblRingerOff->hide();
        ui->lblCountdown->hide();
        ui->lblUntilLabel->hide();
        ui->lblRingerOn->show();
    }
    else {
        ui->lblRingerOn->hide();
        ui->lblRingerOff->show();
        ui->lblCountdown->show();
        ui->lblUntilLabel->show();
    }
}

void MainWindow::setUntil(QString txt)
{
    qDebug("set Until!");
    ui->lblUntilLabel->setText(txt);
}

void MainWindow::setCountdown(QString txt)
{
    qDebug("set Countdown!");
    ui->lblCountdown->setText(txt);
}

void MainWindow::setLastCall(QString txtWho, QString txtNumber, QString txtWhen)
{
    ui->lblCallerName->setText(txtWho);
    ui->lblPhoneNumber->setText(txtNumber);
    ui->lblLastCallLabel->setText(txtWhen);
}

