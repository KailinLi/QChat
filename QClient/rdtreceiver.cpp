#include "rdtreceiver.h"

RdtReceiver::RdtReceiver(QObject *parent):
    QUdpSocket(parent)
{

}

void RdtReceiver::initThread(QHostAddress &destination, quint16 destinationPort)
{
    thread =  new RdtReceiverThread(this, destination, destinationPort);
    connect (thread, &RdtReceiverThread::finished, thread, &RdtReceiverThread::deleteLater);
    thread->start ();
}
