#include "displaymanager.h"
#include <syslog.h>
#include <math.h>


DisplayManager::DisplayManager(QObject *parent) :
    QObject(parent)
{
    m_bDndActive = false;
    m_clockTimer.setInterval(1000);
    connect(&m_clockTimer, SIGNAL(timeout()), this, SLOT(updateDisplay()));
    m_clockTimer.start();

    connect(this, SIGNAL(needsUpdate()), SLOT(updateDisplay()), Qt::QueuedConnection);
}

void DisplayManager::setDndStatus( bool bEnabled )
{
    m_bDndActive = bEnabled;
    m_bDisplayDirty = true;
    emit needsUpdate();
}

void DisplayManager::setDndEndTime( QDateTime endTime )
{
    m_dndEndTime = endTime;
    m_bDisplayDirty = true;
    emit needsUpdate();
}

void DisplayManager::updateDisplay()
{
    if( m_bDndActive ) {
        int secsUntil = QDateTime::currentDateTime().secsTo( m_dndEndTime.addSecs(59) );
        int hours = (secsUntil / 60 / 60) % 24;
        int minutes = (secsUntil / 60) % 60;
        // int seconds = secsUntil % 60;

        if( hours != m_iCountDownHours || minutes != m_iCountDownMinutes ) {
            m_iCountDownHours = hours;
            m_iCountDownMinutes = minutes;
            m_bCountdownDirty = true;
        }
    }

    if( m_bDisplayDirty ) drawAll();
    else if( m_bCountdownDirty ) drawCountdown();
}

void DisplayManager::drawAll()
{
    qDebug("Redrawing screen");
    emit ringEnabledChanged(!m_bDndActive);
    if( m_bDndActive ) {
        QString strTime = QString("Until %1").arg(m_dndEndTime.toString("h:mm ap"));
        emit untilChanged(strTime);
        qDebug( strTime.toAscii() );
        drawCountdown();
    } else {
        emit untilChanged(QString(""));
        emit countdownChanged(QString(""));
    }
    m_bDisplayDirty = false;
}

void DisplayManager::drawCountdown()
{
    QString strCountdown = QString("%1:%2").arg(m_iCountDownHours).arg(m_iCountDownMinutes, 2, 10, QChar('0') );
    emit countdownChanged(strCountdown);
    qDebug(strCountdown.toAscii());
    m_bCountdownDirty = false;
}
