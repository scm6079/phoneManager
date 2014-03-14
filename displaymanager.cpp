#include "displaymanager.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <syslog.h>
#include <math.h>


#include "targa.h"

DisplayManager::DisplayManager(QObject *parent) :
    QObject(parent)
{
    m_bDndActive = false;

    m_clockTimer.setInterval(1000);
    connect(&m_clockTimer, SIGNAL(timeout()), this, SLOT(updateDisplay()));
    m_clockTimer.start();

    connect(this, SIGNAL(needsUpdate()), SLOT(updateDisplay()), Qt::QueuedConnection);

    loadImgFromFile(m_imgBackgroundDndActive, QString("resources/bgActive.tga"));
    loadImgFromFile(m_imgBackgroundDndInactive, QString("resources/bgInactive.tga"));
    loadImgFromFile(m_imgCountdownColon, QString("resources/cr74-:.tga"));
    loadImgFromFile(m_imgDndEndTimeLabel, QString("resources/cr23-Until.tga"));
    loadImgFromFile(m_imgDndEndTimeColon, QString("resources/cr23-:.tga"));
    loadImgFromFile(m_imgDndEndTimeP, QString("resources/cr23-p.tga"));
    loadImgFromFile(m_imgDndEndTimeA, QString("resources/cr23-a.tga"));
    loadImgFromFile(m_imgDndEndTimeM, QString("resources/cr23-m.tga"));

    for( int i=0; i < 10; i++ ) {
        loadImgFromFile(m_aImgCountdownNumbers[i], QString("resources/cr74-%1.tga").arg(i));
        loadImgFromFile(m_aImgDndEndTimeNumbers[i], QString("resources/cr23-%1.tga").arg(i));
    }

    m_bFbInitialized = false;
    //open framebuffer device and read out info
    m_fdFb = open("/dev/fb0",O_RDWR);
    if( m_fdFb ) {
        m_bFbInitialized = true;
        ioctl(m_fdFb, FBIOGET_VSCREENINFO, &m_screeninfo);
        if ( m_screeninfo.bits_per_pixel <= 8 ) {
           syslog(LOG_ERR, "[DisplayManager] palettized / low color modes not supported");
           m_bFbInitialized = false;
        }
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
        writeImageToFramebuffer( m_imgBackgroundDndActive, 0, 0 );
        drawCountdown();
    } else {
        qDebug("Ringer On");
        writeImageToFramebuffer( m_imgBackgroundDndInactive, 0, 0 );
    }
    m_bDisplayDirty = false;
}

void DisplayManager::drawCountdown()
{
    qDebug("Remaining %d:%d", m_iCountDownHours, m_iCountDownMinutes);
    m_bCountdownDirty = false;
}

void DisplayManager::loadImgFromFile( TargaImage& img, const QString& strFileName )
{
    if( !LoadTarga(strFileName.toAscii().constData(), img.wWidthOfImage, img.wHeightOfImage, img.wDepthInBytes, &img.aImageData ) ) {
        qDebug("Image loading failed for %s", strFileName.toAscii().constData());
        syslog(LOG_ERR, "[DisplayManager] could not load resource %s", strFileName.toAscii().constData());
    } else {
        qDebug("Loaded resource %s", strFileName.toAscii().constData());
    }
}

void DisplayManager::writeImageToFramebuffer(TargaImage& img, int x, int y)
{
    BYTE *aDeviceImage = NULL;
    BYTE *pPixelSource = NULL;
    BYTE *pPixelDestination = NULL;
    DWORD dwPixel;
    DWORD dwDestBytesPerPixel;
    DWORD dwDestImageSize;

    if( !m_bFbInitialized ) return;

    // allocate a buffer for converting a device-dependent image
    dwDestBytesPerPixel = (DWORD)ceil((double)m_screeninfo.bits_per_pixel / 8.0);
    dwDestImageSize = dwDestBytesPerPixel * m_screeninfo.xres * m_screeninfo.yres;
    aDeviceImage = (BYTE *)malloc(dwDestImageSize);
    if ( aDeviceImage == NULL ) {
       syslog(LOG_ERR, "[DisplayManager] could not allocate buffer for device dependent image");
       return;
    }
    memset(aDeviceImage, 0, dwDestImageSize);

    // convert the source image data to a device-dependent image
    unsigned long lCurrentY = 0;
    while ( lCurrentY < m_screeninfo.yres ) {
       unsigned long lCurrentX = 0;
       while ( lCurrentX < m_screeninfo.xres ) {

          // copy pixels if in bounds
          if ( lCurrentX < img.wWidthOfImage && lCurrentY < img.wHeightOfImage ) {

             // get the source pixel
             pPixelSource = &img.aImageData[(lCurrentX + (lCurrentY * img.wWidthOfImage)) * img.wDepthInBytes];

             // convert the source pixel to the target color format
             BYTE yRed   = ((DWORD)(pPixelSource[0]) >> (8 - m_screeninfo.red.length));
             BYTE yGreen = ((DWORD)(pPixelSource[1]) >> (8 - m_screeninfo.green.length));
             BYTE yBlue  = ((DWORD)(pPixelSource[2]) >> (8 - m_screeninfo.blue.length));
             BYTE yAlpha = 0;

             if ( img.wDepthInBytes > 3 ) {
                yAlpha = ((DWORD)(pPixelSource[3]) >> (8 - m_screeninfo.transp.length));
             }

             dwPixel = (yRed << m_screeninfo.red.offset) | (yGreen << m_screeninfo.green.offset) | (yBlue << m_screeninfo.blue.offset) | (yAlpha << m_screeninfo.transp.offset);

             // copy the converted pixel to the correct buffer destination
             unsigned long lCurrent = lCurrentX + (lCurrentY * m_screeninfo.xres);
             DWORD dwDestinationBit = lCurrent * m_screeninfo.bits_per_pixel;
             DWORD dwDestinationByte = dwDestinationBit / 8;
             DWORD dwDestinationOffset = dwDestinationBit - (dwDestinationByte * 8);

             pPixelDestination = &aDeviceImage[dwDestinationByte];

             dwPixel <<= dwDestinationOffset;
             dwPixel <<= 32 - (m_screeninfo.red.length + m_screeninfo.green.length + m_screeninfo.blue.length + m_screeninfo.transp.length);

             *((DWORD *)pPixelDestination) = (*((DWORD *)pPixelDestination)) | dwPixel;
          }

          // next pixel
          lCurrentX++;
       }
       lCurrentY++;
    }

    // embed framebuffer into memory
    unsigned char *pScreenData = (unsigned char*)mmap(0, dwDestImageSize, PROT_READ | PROT_WRITE, MAP_SHARED, m_fdFb, 0);

    // copy data
    memcpy(pScreenData, aDeviceImage, dwDestImageSize);

    // mask framebuffer out of memory
    munmap(pScreenData, m_screeninfo.xres * m_screeninfo.yres * dwDestBytesPerPixel);

    // free source data
    free(aDeviceImage);
}
