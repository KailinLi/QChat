#include "rdtsender.h"

RdtSender::RdtSender(QObject *parent):
    QUdpSocket(parent),
    receiver(new QUdpSocket(this))
{
    connect (this, &RdtSender::canSend, this, &RdtSender::sendPiece);
}

//void RdtSender::setSender(const QHostAddress &address, quint16 port)
//{
//    this->destination = address;
//    this->destinationPort = port;
//    this->data = 10;
//    this->current = 0;
//}

//void RdtSender::setRcv(const QHostAddress &address, quint16 port)
//{
//    receiver->bind (address, port, QUdpSocket::ShareAddress);
//    connect (receiver, &QUdpSocket::readyRead, this, &RdtSender::rdtRcv);
//    state = Send;
//}

void RdtSender::sendData()
{
    while (state != Finish) {
        if (state == Send) {
            state = Wait;
            emit canSend ();
        }
    }
    receiver->deleteLater ();
    deleteLater ();
    qDebug() << "ok";
}

void RdtSender::sendPiece()
{
    state = Wait;
    QByteArray datagram;
    qDebug() << tr("send %1").arg (current);
    datagram.setNum (current++);
    writeDatagram (datagram, destination, destinationPort);
}

void RdtSender::rdtRcv()
{
    QByteArray dataGram;
    while (receiver->hasPendingDatagrams ()) {
        dataGram.resize (receiver->pendingDatagramSize ());
        receiver->readDatagram (dataGram.data (), dataGram.size ());
        qDebug() << dataGram;
    }
    QString str = QString::fromUtf8(dataGram);
    emit updateProgress(str.toInt ());
    if (str.toInt () == 10000) state = Finish;
    else state = Send;
}

void RdtSender::setFile(QFile *file)
{
    this->file = file;
}

void RdtSender::setDestination(QHostAddress &destination, quint16 destinationPort)
{
    this->destination = destination;
    this->destinationPort = destinationPort;
    this->data = 10;
    this->current = 0;
}

void RdtSender::bindListen(QHostAddress &address, quint16 port)
{
    receiver->bind (address, port);
    connect (receiver, &QUdpSocket::readyRead, this, &RdtSender::rdtRcv);
    state = Send;
}
