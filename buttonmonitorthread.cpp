#include "buttonmonitorthread.h"
#include <unistd.h>
#include <syslog.h>
#include <stdint.h>

#define PIFACE_HWADDR 0

ButtonMonitorThread::ButtonMonitorThread(QObject *parent) :
    QThread(NULL)
{
    moveToThread(this);

    connect((QObject *)parent, SIGNAL(destroyed()), this, SLOT(quit()));


    debounceTimer.setInterval(20);
    debounceTimer.setSingleShot(true);
    connect(&debounceTimer, SIGNAL(timeout()), this, SLOT(onDebounce()));

    pressTimer.setInterval(50);
    pressTimer.setSingleShot(true);
    connect(&pressTimer, SIGNAL(timeout()), this, SLOT(onReadAttempt()));
    connect(this, SIGNAL(tryReading()), &pressTimer, SLOT(start()));
}

ButtonMonitorThread::~ButtonMonitorThread()
{
    pifacedigital_close(PIFACE_HWADDR);
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
    pifacedigital_open(PIFACE_HWADDR);

    if( pifacedigital_enable_interrupts() ) {
        qDebug("Could not enable interrupts.  Try running using sudo to enable PiFaceDigital interrupts.\n");
        syslog(LOG_ERR, "[ButtonMonitorThread] Could not enable interrupts.");
    }

    emit tryReading();
}

void ButtonMonitorThread::setOutputState( int iOutput, bool bState )
{
    qDebug("Setting output %d to %d", iOutput, (int)bState);
    pifacedigital_digital_write(iOutput, bState);
}

void ButtonMonitorThread::splitInputs( uint8_t inputs ) 
{
    m_bCurrentButtonState[0] = !(inputs & 0x1);
    m_bCurrentButtonState[1] = !(inputs & 0x2);
    m_bCurrentButtonState[2] = !(inputs & 0x4);
    m_bCurrentButtonState[3] = !(inputs & 0x8);
    m_bCurrentButtonState[4] = !(inputs & 0x10);
    m_bCurrentButtonState[5] = !(inputs & 0x20);
    m_bCurrentButtonState[6] = !(inputs & 0x40);
    m_bCurrentButtonState[7] = !(inputs & 0x80);
}

void ButtonMonitorThread::onDebounce()
{
    uint8_t inputs = pifacedigital_read_reg(0x13, PIFACE_HWADDR);
    splitInputs( inputs );

    qDebug("Debounced Inputs: 0x%x\n", inputs);

    for( int i=0; i < 8; i++ ) {
        if ( m_bNeedsDebounced[i] ) {
            if( m_bCurrentButtonState[i] != m_aLastButtonState[i] ) {
                m_aLastButtonState[i] = m_bCurrentButtonState[i];                
                emit buttonChange(i, m_bCurrentButtonState[i]);
            }

            m_bNeedsDebounced[i] = false;
        }
    }

    emit tryReading();
}

void ButtonMonitorThread::onReadAttempt()
{
    bool bDebounce = false;

    uint8_t inputs = pifacedigital_read_reg(0x13, PIFACE_HWADDR);
    splitInputs( inputs );

    for( int i=0; i < 8; i++ ) {
        if( m_bCurrentButtonState[i] != m_aLastButtonState[i] ) {
            m_bNeedsDebounced[i] = true;
            bDebounce = true;
        }
    }

    if ( bDebounce ) debounceTimer.start();
    else emit tryReading(); 
}

