#include "ncidclient.h"
#include <QtCore/QStringList>

NcidClient::NcidClient(QObject* parent): QTcpSocket(parent)
{
    connect(this, SIGNAL(readyRead()), this, SLOT(readData()));
}

/**
 * reads one line from the ncid server
 */
void NcidClient::readData()
{
    int size = 1024;
    char data[size];
    this->readLine (data, size);
    parseLine(QString(data));
    if (this->canReadLine()) readData();
}

/**
 * Checks for type of line recived from ncid server and hands
 * it over to the line specific parsing function.
 *
 * @param line The line to parse
 */
void NcidClient::parseLine ( QString line )
{
    line = line.simplified();
    qDebug() << "parseLine():" << line;
    if ( line.startsWith ( "200" ) ) {
        emit connected(true);
    } else if ( line.startsWith ( "300" ) ) {
        // end of call log
    } else if ( line.startsWith ( "CIDLOG:" ) ) {
        parseCID(line);
    } else if ( line.startsWith ( "CIDINFO:" ) ) {
        // ringing
    } else if ( line.startsWith ( "CID:" ) ) {
        parseCID(line);
    } else {
        // not handeled
    }
}

/**
 * Parses CID and CIDLOG lines with the following format:
 * CID: *DATE*18031012*TIME*1543*LINE*9876543210*NMBR*0987654321*MESG*NONE*NAME*NO NAME*
 * CIDLOG: *DATE*11041012*TIME*1231*LINE*9876543210*NMBR*Anonym*MESG*NONE*NAME*NO NAME*
 *
 * @param line The line to parse
 */
void NcidClient::parseCID(QString line)
{
    CallInfo entry;
    QDate date = QDate(0,0,0);
    QTime time = QTime(0,0);
    QStringList f = line.split("*");
    for (int i=1; i<f.length()-1; i+=2) {
        if (f[i].compare("DATE",Qt::CaseInsensitive) == 0) {
            date.setDate(f[i+1].right(4).toInt(),f[i+1].mid(2,2).toInt(),f[i+1].left(2).toInt());
        }
        else if (f[i].compare("TIME",Qt::CaseInsensitive) == 0) {
            time = QTime::fromString(f[i+1],"hhmm");
        }
        else if (f[i].compare("LINE",Qt::CaseInsensitive) == 0) {
            entry.phoneLine = f[i+1];
        }
        else if (f[i].compare("NMBR",Qt::CaseInsensitive) == 0) {
            entry.phoneNmbr = f[i+1];
        }
        else if (f[i].compare("NAME",Qt::CaseInsensitive) == 0) {
            entry.name = f[i+1];
        }
        else if (f[i].compare("MESG",Qt::CaseInsensitive) == 0) {
            entry.msg = f[i+1];
        }
        else {
            qDebug() << "parseCID(): Unkown part:" << f[i+1];
        }
    }
    entry.date = QDateTime(date, time);
    emit newCallInfo(entry);

    qDebug() << "parseCID(): Number:" << entry.phoneNmbr << "Time:" << entry.date.toString();

    if (line.startsWith("CID:",Qt::CaseInsensitive)) emit incomingCall(entry);
}

