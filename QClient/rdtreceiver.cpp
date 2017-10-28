#include "rdtreceiver.h"


RdtReceiver::RdtReceiver(QObject *parent):
    QUdpSocket(parent),
    sender(new QUdpSocket(this))
{

}

void RdtReceiver::receive()
{
    QByteArray dataGram;
    while (hasPendingDatagrams ()) {
        dataGram.resize (pendingDatagramSize ());
        readDatagram (dataGram.data (), dataGram.size ());
        qDebug() << dataGram;
    }

    sender->writeDatagram (dataGram, destination, destinationPort);
    QString str = QString::fromUtf8(dataGram);
    emit updateProgress(str.toInt ());
    if (str.toInt () == 10000) {
        deleteLater ();
        sender->deleteLater ();
        emit finish ();
    }
}

void RdtReceiver::setFile(QFile *file)
{
    this->file = file;
}

void RdtReceiver::setDestination(QHostAddress &destination, quint16 destinationPort)
{
    this->destination = destination;
    this->destinationPort = destinationPort;
}

void RdtReceiver::bindListen(QHostAddress &address, quint16 port)
{
    bind (address, port);
    connect (this, &RdtReceiver::readyRead, this, &RdtReceiver::receive);
}
