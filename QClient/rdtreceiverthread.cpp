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

void RdtReceiverThread::sendACK(qint64 sequence)
{
    QDataStream stream(&block, QIODevice::WriteOnly);
    stream.setVersion (QDataStream::Qt_5_6);
    stream << sequence;
    sender->writeDatagram (block.constData (), block.size (), destination, destinationPort);
    block.resize (0);
}
