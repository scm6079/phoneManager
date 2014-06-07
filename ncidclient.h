#ifndef NCIDCLIENT_H
#define NCIDCLIENT_H

#include <QtNetwork/QTcpSocket>

#include <QtCore/QString>
#include <QtCore/QDateTime>

class NcidClient : public QTcpSocket
{
    Q_OBJECT
public:
    struct CallInfo {
        QDateTime date;
        QString phoneNmbr;
        QString phoneLine;
        QString msg;
        QString name;
    };

    explicit NcidClient(QObject* parent = 0);
    static QString logEntryToString(const NcidClient::CallInfo);

private:
    QString ncidHostIP;
    int ncidHostPort;

    void parseCID(const QString);
    void parseLine(QString);

private slots:
    void readData();

signals:
    void connected(bool);
    void newCallInfo(const NcidClient::CallInfo);
    void incomingCall(const NcidClient::CallInfo);
};

#endif // NCIDCLIENT_H

