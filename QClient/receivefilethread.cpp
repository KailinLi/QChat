#include "receivefilethread.h"

ReceiveFileThread::ReceiveFileThread(QObject *parent):
    QThread(parent)
//    receiving(true)
{

}

void ReceiveFileThread::run()
{
    receiver = new RdtReceiver();
    receiver->setFile (file, fileSize);
    receiver->setDestination (destination, destinationPort);
    receiver->bindListen (address, port);
    connect (receiver, &RdtReceiver::updateProgress, this, &ReceiveFileThread::updateProcess, Qt::QueuedConnection);
    connect (receiver, &RdtReceiver::finish, this, &ReceiveFileThread::finishReceive, Qt::QueuedConnection);
    exec();
//    while(receiving);
    receiver->deleteLater ();
    qDebug() << "finish receiver";
}

void ReceiveFileThread::stop()
{
    quit();
//    receiving = false;
}

void ReceiveFileThread::setFile(QFile *file, qint64 fileSize)
{
    this->file = file;
    this->fileSize = fileSize;
}

void ReceiveFileThread::setDestination(QHostAddress &destination, quint16 destinationPort)
{
    this->destination = destination;
    this->destinationPort = destinationPort;
}

void ReceiveFileThread::setAddress(QHostAddress &address, quint16 port)
{
    this->address = address;
    this->port = port;
}
