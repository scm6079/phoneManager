#include "phonemanager.h"

#define BTN_INCREMENT 0
#define BTN_DECREMENT 1
#define BTN_CANCEL    2
#define INCREMENT_PERIOD  900
#define INITIAL_INCREMENT 900
#define REPEAT_INTERVAL   1000


PhoneManager::PhoneManager(QObject *parent, MainWindow *pMainWindow) :
    QObject(parent)
{
    m_ncidClient = NULL;
    m_connected = false;
    m_settings = new QSettings;
    loadConfiguration();
    m_callReported = false;

    m_pButtonMonitorThread = new ButtonMonitorThread(this);
    connect(m_pButtonMonitorThread, SIGNAL(buttonChange(int,bool)), this, SLOT(onButtonChange(int,bool)));
    connect(this, SIGNAL(setOutputState(int,bool)), m_pButtonMonitorThread, SLOT(setOutputState(int,bool)));

    m_iConsecutiveDecrement = 0;
    m_bDndActive = false;
    m_dndEndTime = QDateTime::fromTime_t( 0 );

    m_clockTimer.setInterval(1000);
    connect(&m_clockTimer, SIGNAL(timeout()), this, SLOT(onClockTick()));
    m_clockTimer.start();

    m_btnIncrementTimer.setInterval(REPEAT_INTERVAL);
    m_btnDecrementTimer.setInterval(REPEAT_INTERVAL);
    connect(&m_btnIncrementTimer, SIGNAL(timeout()), this, SLOT(onIncrementDnd()));
    connect(&m_btnDecrementTimer, SIGNAL(timeout()), this, SLOT(onDecrementDnd()));

    m_pDisplayManager = new DisplayManager(this);
    connect(this, SIGNAL(dndStatus(bool)), m_pDisplayManager, SLOT(setDndStatus(bool)));
    connect(this, SIGNAL(dndEndTime(QDateTime)), m_pDisplayManager, SLOT(setDndEndTime(QDateTime)));

    connect( m_pDisplayManager, SIGNAL(ringEnabledChanged(bool)), pMainWindow, SLOT(setRinger(bool)) );
    connect( m_pDisplayManager, SIGNAL(untilChanged(QString)), pMainWindow, SLOT(setUntil(QString)) );
    connect( m_pDisplayManager, SIGNAL(countdownChanged(QString)), pMainWindow, SLOT(setCountdown(QString)) );
    connect( this, SIGNAL(lastCallChanged(QString, QString, QString)), pMainWindow, SLOT(setLastCall(QString, QString, QString)) );

    m_pButtonMonitorThread->start();

    disableDnd();
    connectToNcidServer();
}

PhoneManager::~PhoneManager()
{
    if (m_pButtonMonitorThread) {
       delete m_pButtonMonitorThread;
       m_pButtonMonitorThread = NULL;
    }
    if (m_ncidClient && m_ncidClient->isOpen()) {
        m_ncidClient->close();
    }
}

void PhoneManager::onIncrementDnd()
{
    if( !m_bDndActive ) {
        qDebug("Receiving inital increment request.");
        m_dndEndTime = QDateTime::currentDateTime().addSecs(INITIAL_INCREMENT);
        qDebug( "%s", m_dndEndTime.toString().toStdString().c_str() );
        enableDnd();
    }
    else {
        m_dndEndTime = m_dndEndTime.addSecs(INCREMENT_PERIOD);
    }

    emit dndEndTime( m_dndEndTime );
}

void PhoneManager::onDecrementDnd()
{
    m_iConsecutiveDecrement++;
    if( m_bDndActive ) {
        m_dndEndTime = m_dndEndTime.addSecs(-INCREMENT_PERIOD);
        if( m_iConsecutiveDecrement > 6 ) {
            // Cancel request
            disableDnd();
        }
    }

    emit dndEndTime( m_dndEndTime );
}

void PhoneManager::onCancelDnd()
{
    disableDnd();
}

void PhoneManager::enableDnd()
{
    m_bDndActive = true;
    qDebug("Enabling DND Mode");
    emit setOutputState(0, true);
    emit setOutputState(1, true);
    emit dndStatus( m_bDndActive );
}

void PhoneManager::disableDnd()
{
    m_bDndActive = false;
    qDebug("Disabling DND Mode");
    emit setOutputState(0, false);
    emit setOutputState(1, false);
    emit dndStatus( m_bDndActive );
}

void PhoneManager::onButtonChange(int iBtn, bool bState)
{
    qDebug( "Button %d now %d", iBtn, (int)bState );

    if(iBtn == BTN_INCREMENT) {
        if( bState ) {
            onIncrementDnd();
            m_btnIncrementTimer.start();
        }
        else {
            m_btnIncrementTimer.stop();
        }
    }
    if(iBtn == BTN_DECREMENT) {
        if( bState ) {
            onDecrementDnd();
            m_btnDecrementTimer.start();
        }
        else {
            m_iConsecutiveDecrement = 0;
            m_btnDecrementTimer.stop();
        }
    }
}


void PhoneManager::onClockTick()
{
    if( m_bDndActive ) {
        // Is it time?
        if( m_dndEndTime < QDateTime::currentDateTime() ) {
            qDebug("End time reached");
            disableDnd();
        }
    }
}

void PhoneManager::loadConfiguration()
{
    m_ncidHostIP = m_settings->value("ncidserver/ip","127.0.0.1").toString();
    m_ncidHostPort = m_settings->value("ncidserver/port",3333).toInt();
}

void PhoneManager::ncidServerConnected(bool c)
{
    m_connected = c;
}

void PhoneManager::connectToNcidServer()
{
    if (m_ncidClient) {
        m_ncidClient->close();
        m_ncidClient->deleteLater();
        m_connected = false;
    }

    m_ncidClient = new NcidClient;
    m_ncidClient->connectToHost (m_ncidHostIP, m_ncidHostPort, QIODevice::ReadOnly );
    connect(m_ncidClient, SIGNAL(newCallInfo(NcidClient::CallInfo)), this, SLOT(loggedCall(NcidClient::CallInfo)));
    connect(m_ncidClient, SIGNAL(incomingCall(NcidClient::CallInfo)), this, SLOT(incomingCall(NcidClient::CallInfo)));

    if (!m_ncidClient->waitForConnected(1000)) {
        qDebug() << "NCID Client Connection Error";
        m_ncidClient->deleteLater();
    }
}

void PhoneManager::displayCall(const NcidClient::CallInfo call)
{
   QString phone = call.phoneNmbr;
   if( phone.length() == 10 ) {
      phone.insert(6, QChar('-'));
      phone.insert(3, QChar(' '));
      phone.insert(3, QChar(')'));
      phone.insert(0, QChar('('));
   }

   emit lastCallChanged(call.name, phone, QString("Last Call %1").arg(call.date.toString(QString("ddd h:mmap"))));
}

void PhoneManager::loggedCall(const NcidClient::CallInfo call)
{
    if( !m_callReported ) {
       displayCall( call );
    }
}

void PhoneManager::incomingCall(const NcidClient::CallInfo call)
{
    // TODO: Determine if caller should trigger phone ring through with double call within time-period

    m_callReported = true;
    displayCall( call );
}


