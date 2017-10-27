#include "rdtsender.h"

RdtSender::RdtSender(QObject *parent):
    QUdpSocket(parent),
    rcvSocket(new QUdpSocket(this))
{
    connect (this, &RdtSender::canSend, this, &RdtSender::sendPiece);
}

void RdtSender::setSender(const QHostAddress &address, quint16 port)
{
    this->address = address;
    this->port = port;
    this->data = 10;
    this->current = 0;
}

void RdtSender::setRcv(const QHostAddress &address, quint16 port)
{
    rcvSocket->bind (address, port, QUdpSocket::ShareAddress);
    connect (rcvSocket, &QUdpSocket::readyRead, this, &RdtSender::rdtRcv);
    state = Send;
}

void RdtSender::sendData()
{
    while (state != Finish) {
        if (state == Send) {
            state = Wait;
            emit canSend ();
        }
    }
    rcvSocket->deleteLater ();
    deleteLater ();
    qDebug() << "ok";
}

void RdtSender::sendPiece()
{
    state = Wait;
    QByteArray datagram;
    qDebug() << tr("send %1").arg (current);
    datagram.setNum (current++);
    writeDatagram (datagram, address, port);
}

void RdtSender::rdtRcv()
{
    QByteArray dataGram;
    while (rcvSocket->hasPendingDatagrams ()) {
        dataGram.resize (rcvSocket->pendingDatagramSize ());
        rcvSocket->readDatagram (dataGram.data (), dataGram.size ());
        qDebug() << dataGram;
    }
    QString str = QString::fromUtf8(dataGram);
    emit step(str.toInt ());
    if (str.toInt () == 10000) state = Finish;
    else state = Send;
}
