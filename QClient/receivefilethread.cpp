#include "receivefilethread.h"


ReceiveFileThread::ReceiveFileThread(QObject *parent, QFile *file,
                                     QHostAddress &address, quint16 port,
                                     QHostAddress &destination, quint16 destinationPort):
    QThread(parent),
    listening(true)
{
    receiver = new RdtReceiver(this);
    receiver->setFile (file);
    receiver->bind (address, port);
    receiver->initThread (destination, destinationPort);
}

void ReceiveFileThread::run()
{
    while (listening) {

    }
    if (receiver) {
        receiver->close ();
        receiver->deleteLater ();
    }
}

void ReceiveFileThread::stopListen()
{
    listening = false;
}


