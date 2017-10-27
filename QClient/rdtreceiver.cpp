#include "rdtreceiver.h"


RdtReceiver::RdtReceiver(QObject *parent, const QHostAddress &address, quint16 port):
    QUdpSocket(parent),
    address(address),
    port(port),
    rcvSocket(new QUdpSocket(this))
{
    rcvSocket->bind (QHostAddress("127.0.0.1"), 6001);
    connect (rcvSocket, &QUdpSocket::readyRead, this, &RdtReceiver::receive);
}

void RdtReceiver::receive()
{
    QByteArray dataGram;
    while (rcvSocket->hasPendingDatagrams ()) {
        dataGram.resize (rcvSocket->pendingDatagramSize ());
        rcvSocket->readDatagram (dataGram.data (), dataGram.size ());
        qDebug() << dataGram;
    }

    writeDatagram (dataGram, address, port);
    QString str = QString::fromUtf8(dataGram);
    emit step(str.toInt ());
    if (str.toInt () == 10000) {
        rcvSocket->deleteLater ();
        deleteLater ();
        emit finish ();
    }
}
