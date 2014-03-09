#include "buttonmonitorthread.h"
#include <unistd.h>
#include <QTextStream>

ButtonMonitorThread::ButtonMonitorThread(QObject *parent) :
    QThread(NULL)
{
    moveToThread(this);

    //m_bLastStateIncrement = false;
    //m_bLastStateDecrement = false;
    //m_bLastStateCancel = false;

    connect((QObject *)parent, SIGNAL(destroyed()), this, SLOT(quit()));
    connect(this, SIGNAL(tryReading()), SLOT(onReadAttempt()), Qt::QueuedConnection);

}

ButtonMonitorThread::~ButtonMonitorThread()
{

}

void ButtonMonitorThread::run()
{
    openButtons();
    exec();
}

void ButtonMonitorThread::openButtons()
{
    for( int i=0; i < NUM_BUTTONS; i++ ) {
        m_aLastButtonState[i] = false;
    }
    // Open buttons here (connect to PiFace)

    emit tryReading();
}

void ButtonMonitorThread::setOutputState( int iOutput, bool bState )
{
    qDebug("Setting output %d to %d", iOutput, (int)bState);
    //TODO
}

void ButtonMonitorThread::onReadAttempt()
{
    bool btns[8];
    QTextStream qtin(stdin);
    QString btn = qtin.read(1);
    for( int i=0; i < 8; i++ ) btns[i] = m_aLastButtonState[i];
    if( btn.compare("0") == 0 ) btns[0] = !m_aLastButtonState[0];
    if( btn.compare("1") == 0 ) btns[1] = !m_aLastButtonState[1];
    if( btn.compare("2") == 0 ) btns[2] = !m_aLastButtonState[2];
    if( btn.compare("3") == 0 ) btns[3] = !m_aLastButtonState[3];
    if( btn.compare("4") == 0 ) btns[4] = !m_aLastButtonState[4];
    if( btn.compare("5") == 0 ) btns[5] = !m_aLastButtonState[5];
    if( btn.compare("6") == 0 ) btns[6] = !m_aLastButtonState[6];
    if( btn.compare("7") == 0 ) btns[7] = !m_aLastButtonState[7];

    for( int i=0; i < 8; i++ ) {
        if( btns[i] != m_aLastButtonState[i] ) {
            emit buttonChange( i, btns[i] );
        }
        m_aLastButtonState[i] = btns[i];
    }

    emit tryReading();
}

