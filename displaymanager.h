#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#include <QObject>
#include <QDateTime>
#include <QTimer>
#include "targa.h"

class TargaImage {
    WORD wWidthOfImage = 0;
    WORD wHeightOfImage = 0;
    WORD wDepthInBytes = 0;
    BYTE *aImageData = NULL;
};

class DisplayManager : public QObject
{
    Q_OBJECT
public:
    explicit DisplayManager(QObject *parent = 0);

signals:
    void needsUpdate();

public slots:
   void setDndStatus( bool bEnabled );
   void setDndEndTime( QDateTime endTime );
   void updateDisplay();

private:
   void drawAll();
   void drawCountdown();
   void loadImgFromFile( TargaImage&, const char* );
   void writeImageToFramebuffer(TargaImage&, int x, int y);
   QTimer m_clockTimer;
   bool m_bDndActive;
   QDateTime m_dndEndTime;
   int m_iCountDownHours;
   int m_iCountDownMinutes;
   bool m_bDisplayDirty;
   bool m_bCountdownDirty;

   TargaImage m_imgBackgroundDndActive;
   TargaImage m_imgBackgroundDndInactive;
   TargaImage m_imgCountdownColon;
   TargaImage m_aImgCountdownNumbers[10];
   TargaImage m_imgDndEndTimeLabel;
   TargaImage m_imgDndEndTimeColon;
   TargaImage m_imgDndEndTimeP;
   TargaImage m_imgDndEndTimeA;
   TargaImage m_imgDndEndTimeM;
   TargaImage m_aImgDndEndTimeNumbers[10];


};

#endif // DISPLAYMANAGER_H
