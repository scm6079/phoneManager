#ifndef BUTTONMONITORTHREAD_H
#define BUTTONMONITORTHREAD_H

#include <QThread>
#include <QDateTime>
#include <QTimer>
#include "pifacedigital.h"

#define NUM_BUTTONS 8

class ButtonMonitorThread : public QThread
{
    Q_OBJECT
public:
    explicit ButtonMonitorThread(QObject *parent = 0);
    virtual ~ButtonMonitorThread();

    void run();

signals:
    void tryReading();
    void buttonChange(int iBtn, bool bState);

public slots:
    void onReadAttempt();
    void openButtons();
    void setOutputState( int iOutput, bool bState );
    void onDebounce();

private:
    void splitInputs( uint8_t inputs ) ;
    QTimer debounceTimer;
    QTimer pressTimer;
    bool m_aLastButtonState[8];
    bool m_bNeedsDebounced[8];
    bool m_bCurrentButtonState[8];
    QDateTime m_dtDebounceFrom[8];
};

#endif // BUTTONMONITORTHREAD_H
