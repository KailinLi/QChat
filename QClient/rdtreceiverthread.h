#ifndef RDTRECEIVERTHREAD_H
#define RDTRECEIVERTHREAD_H

#include <QObject>
#include <QThread>
#include <QtNetwork>

class RdtReceiverThread : public QThread
{
    Q_OBJECT
public:
    RdtReceiverThread(QObject *parent, QHostAddress &destination, quint16 destinationPort);

    void run() Q_DECL_OVERRIDE;
    QUdpSocket *sender;

    volatile bool sending;
    QHostAddress destination;
    quint16 destinationPort;

    void stopSending();

private:
    QByteArray block;
};

#endif // RDTRECEIVERTHREAD_H
