#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#include <QObject>
#include <QDateTime>
#include <QTimer>
#include "targa.h"
//#include <linux/fb.h>

class TargaImage {
public:
    TargaImage() {
        wWidthOfImage = 0;
        wHeightOfImage = 0;
        wDepthInBytes = 0;
        aImageData = NULL;
    }
    WORD wWidthOfImage;
    WORD wHeightOfImage;
    WORD wDepthInBytes;
    BYTE *aImageData;
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
   void loadImgFromFile( TargaImage&, const QString& );
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

   bool m_bFbInitialized;
   int m_fdFb;
   // struct fb_var_screeninfo m_screeninfo;
};

#endif // DISPLAYMANAGER_H
