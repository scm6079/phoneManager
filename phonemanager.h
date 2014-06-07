#ifndef PHONEMANAGER_H
#define PHONEMANAGER_H

#include <QObject>
#include <QDateTime>
#include <QTimer>
#include <QSettings>
#include "buttonmonitorthread.h"
#include "displaymanager.h"
#include "mainwindow.h"
#include "ncidclient.h"

class PhoneManager : public QObject
{
    Q_OBJECT
public:
    explicit PhoneManager(QObject *parent = 0, MainWindow *pMainWindow=0);
    virtual ~PhoneManager();

signals:
    void setOutputState( int iOutput, bool bState );
    void dndStatus( bool bEnabled );
    void dndEndTime( QDateTime endTime );
    void lastCallChanged(QString, QString, QString);

public slots:
    void onButtonChange(int iBtn, bool bState);
    void onIncrementDnd();
    void onDecrementDnd();
    void onCancelDnd();
    void onClockTick();

private slots:
    void loadConfiguration();
    void loggedCall(NcidClient::CallInfo);
    void incomingCall(const NcidClient::CallInfo);
    void ncidServerConnected(bool);
    void displayCall(const NcidClient::CallInfo);

private:
    void enableDnd();
    void disableDnd();
    void connectToNcidServer();

    bool m_bDndActive;
    int  m_iConsecutiveDecrement;
    QDateTime m_dndEndTime;

    QTimer m_clockTimer;
    QTimer m_btnIncrementTimer;
    QTimer m_btnDecrementTimer;
    ButtonMonitorThread *m_pButtonMonitorThread;
    DisplayManager *m_pDisplayManager;

    QSettings *m_settings;
    NcidClient *m_ncidClient;
    QString m_ncidHostIP;
    int m_ncidHostPort;
    bool m_connected;
    bool m_callReported;
};

#endif // PHONEMANAGER_H
