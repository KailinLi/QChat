#ifndef RDTRECEIVER_H
#define RDTRECEIVER_H

#include <QObject>
#include <QtNetwork>

class RdtReceiver : public QUdpSocket
{
    Q_OBJECT
public:
    RdtReceiver(QObject *parent);
    void receive();
private:
    int hasRcv;
    QUdpSocket *sender;
//    QUdpSocket* rcvSocket;
    QFile *file;
    QHostAddress destination;
    quint16 destinationPort;
public:
    void setFile(QFile *file);
    void setDestination(QHostAddress &destination, quint16 destinationPort);
    void bindListen(QHostAddress &address, quint16 port);
signals:
    void finish();
    void updateProgress(qint64);
};

#endif // RDTRECEIVER_H
