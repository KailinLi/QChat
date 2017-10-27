#ifndef RDTRECEIVER_H
#define RDTRECEIVER_H

#include <QObject>
#include <QtNetwork>

class RdtReceiver : public QUdpSocket
{
    Q_OBJECT
public:
    RdtReceiver(QObject *parent, const QHostAddress& address, quint16 port);
    void receive();
private:
    int hasRcv;
    quint16 port;
    QUdpSocket* udpSocket;
    QUdpSocket* rcvSocket;
    QHostAddress address;
signals:
    void finish();
    void step(qint64);
};

#endif // RDTRECEIVER_H
