#include "rdtsenderthread.h"

RdtSenderThread::RdtSenderThread(QObject *parent):
    QThread(parent),
    listening(true)
{
    receiver = new QUdpSocket(this);
    connect (receiver, &QUdpSocket::readyRead, this, &RdtSenderThread::readRdtData);
}

void RdtSenderThread::run()
{
    while (listening) {

    }
    qDebug() << "end listen";
    if (receiver) {
        receiver->close ();
        receiver->deleteLater ();
    }
}

void RdtSenderThread::init(QHostAddress &address, quint16 port, volatile RdtSenderThread::State *state)
{
    receiver->bind (address, port);
    this->state = state;
}

void RdtSenderThread::stopListen()
{
    listening = false;
}

void RdtSenderThread::readRdtData()
{
    if (receiver->bytesAvailable () < static_cast<qint64>(sizeof(qint64))) {
        return;
    }
    QByteArray dataGram;
    dataGram.resize (static_cast<int>(sizeof(qint64)));
    receiver->readDatagram (dataGram.data (), dataGram.size ());
    qint64 sequence = dataGram.toLongLong ();
    *state = State::Send;
}
