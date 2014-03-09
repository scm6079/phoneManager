#ifndef BUTTONMONITORTHREAD_H
#define BUTTONMONITORTHREAD_H

#include <QThread>

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

private:
    bool m_aLastButtonState[8];
};

#endif // BUTTONMONITORTHREAD_H
