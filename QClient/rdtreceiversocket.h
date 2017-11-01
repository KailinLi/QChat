#ifndef RDTRECEIVERSOCKET_H
#define RDTRECEIVERSOCKET_H

#include <QObject>
#include <QtNetwork>

class RdtReceiverSocket : public QUdpSocket
{
public:
    RdtReceiverSocket(QObject *parent = 0, QHostAddress destination = QHostAddress(),
                      quint16 destinationPort = 0);
    ~RdtReceiverSocket();

    void sendACK(qint64 sequenceNumber);
    void deleteSelf();
private:
    QByteArray block;
};

#endif // RDTRECEIVERSOCKET_H
