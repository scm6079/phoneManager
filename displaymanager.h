#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#include <QObject>
#include <QDateTime>
#include <QTimer>


class DisplayManager : public QObject
{
    Q_OBJECT
public:
    explicit DisplayManager(QObject *parent = 0);

signals:
    void needsUpdate();

    void ringEnabledChanged(bool);
    void untilChanged(QString);
    void countdownChanged(QString);
    void lastCallChanged(QString, QString);

public slots:
   void setDndStatus( bool bEnabled );
   void setDndEndTime( QDateTime endTime );
   void updateDisplay();

private:
   void drawAll();
   void drawCountdown();
   QTimer m_clockTimer;
   bool m_bDndActive;
   QDateTime m_dndEndTime;
   int m_iCountDownHours;
   int m_iCountDownMinutes;
   bool m_bDisplayDirty;
   bool m_bCountdownDirty;
};

#endif // DISPLAYMANAGER_H
