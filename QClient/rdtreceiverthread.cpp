#include "rdtreceiverthread.h"

RdtReceiverThread::RdtReceiverThread(QObject *parent, QHostAddress &destination, quint16 destinationPort):
    QThread(parent),
    sending(true),
    destination(destination),
    destinationPort(destinationPort)
{
    sender = new QUdpSocket(this);
}

void RdtReceiverThread::run()
{
    while (sending) {

    }
    if (sender) {
        sender->close ();
        sender->deleteLater ();
    }
}

void RdtReceiverThread::stopSending()
{
    sending = false;
}
