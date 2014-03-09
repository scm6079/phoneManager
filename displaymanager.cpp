#include "displaymanager.h"

DisplayManager::DisplayManager(QObject *parent) :
    QObject(parent)
{
    m_bDndActive = false;

    m_clockTimer.setInterval(1000);
    connect(&m_clockTimer, SIGNAL(timeout()), this, SLOT(updateDisplay()));
    m_clockTimer.start();

    connect(this, SIGNAL(needsUpdate()), SLOT(updateDisplay()), Qt::QueuedConnection);

    loadImgFromFile(m_imgBackgroundDndActive, "bgActive.tga");
    loadImgFromFile(m_imgBackgroundDndInactive, "bgInactive.tga");
    loadImgFromFile(m_imgCountdownColon, "");
    loadImgFromFile(m_imgDndEndTimeLabel, "");
    loadImgFromFile(m_imgDndEndTimeColon, "");
    loadImgFromFile(m_imgDndEndTimeP, "");
    loadImgFromFile(m_imgDndEndTimeA, "");
    loadImgFromFile(m_imgDndEndTimeM, "");

    for( int i=0; i < 10; i++ ) {
        loadImgFromFile(m_aImgCountdownNumbers[i], "");
        loadImgFromFile(m_aImgDndEndTimeNumbers[i], "");
    }

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
    if( m_bDndActive ) {
        qDebug("Silent until %s", m_dndEndTime.toString("h:mm ap").toStdString().c_str() );
        drawCountdown();
    } else {
        qDebug("Ringer On");
    }
    m_bDisplayDirty = false;
}

void DisplayManager::drawCountdown()
{
    qDebug("Remaining %d:%d", m_iCountDownHours, m_iCountDownMinutes);
    m_bCountdownDirty = false;
}

void DisplayManager::loadImgFromFile( TargaImage&, const char* )
{

}

void DisplayManager::writeImageToFramebuffer(TargaImage&, int x, int y)
{

}
