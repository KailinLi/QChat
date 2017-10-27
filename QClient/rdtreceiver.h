#ifndef RDTRECEIVER_H
#define RDTRECEIVER_H

#include <QObject>
#include <QFile>
#include "rdtreceiverthread.h"

class RdtReceiver : public QUdpSocket
{
    Q_OBJECT
public:
    RdtReceiver(QObject *parent);

private:
    RdtReceiverThread *thread;

public:
    void initThread(QHostAddress &destination, quint16 destinationPort);

};

#endif // RDTRECEIVER_H
