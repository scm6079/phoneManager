#ifndef PHONEMANAGER_H
#define PHONEMANAGER_H

#include <QObject>
#include <QDateTime>
#include <QTimer>
#include "buttonmonitorthread.h"
#include "displaymanager.h"

class PhoneManager : public QObject
{
    Q_OBJECT
public:
    explicit PhoneManager(QObject *parent = 0);
    virtual ~PhoneManager();

signals:
    void setOutputState( int iOutput, bool bState );
    void dndStatus( bool bEnabled );
    void dndEndTime( QDateTime endTime );

public slots:
    void onButtonChange(int iBtn, bool bState);
    void onIncrementDnd();
    void onDecrementDnd();
    void onCancelDnd();
    void onClockTick();

private:
    void enableDnd();
    void disableDnd();

    bool m_bDndActive;
    int  m_iConsecutiveDecrement;
    QDateTime m_dndEndTime;

    QTimer m_clockTimer;
    QTimer m_btnIncrementTimer;
    QTimer m_btnDecrementTimer;
    ButtonMonitorThread *m_pButtonMonitorThread;
    DisplayManager *m_pDisplayManager;
};

#endif // PHONEMANAGER_H
