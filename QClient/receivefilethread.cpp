#include "receivefilethread.h"


ReceiveFileThread::ReceiveFileThread(QObject *parent):
    QThread(parent),
    listening(true)
{
    receiver = new RdtReceiver(this);
    connect (receiver, &RdtReceiver::updateProgress, this, &ReceiveFileThread::updateProgress);
}

void ReceiveFileThread::run()
{
    receiver->setFile (file);
    receiver->setTotalSize (fileSize);
    receiver->bind (address, port);
    qDebug() << port;
    receiver->initThread (destination, destinationPort);

    while (listening) {

    }
    if (receiver) {
        receiver->close ();
        receiver->deleteLater ();
    }
}

void ReceiveFileThread::init(QFile *file, QHostAddress &address, quint16 port,
                             QHostAddress &destination, quint16 destinationPort, qint64 fileSize)
{
    this->file = file;
    this->address = address;
    this->port = port;
    this->destination = destination;
    this->destinationPort = destinationPort;
    this->fileSize = fileSize;
}

void ReceiveFileThread::stopListen()
{
    listening = false;
}


