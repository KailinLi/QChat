#include "rdtsenderthread.h"

RdtSenderThread::RdtSenderThread(QObject *parent, QHostAddress &address, quint16 port, volatile State *state):
    QThread(parent),
    listening(true),
    state(state)
{
    receiver = new QUdpSocket(this);
    receiver->bind (address, port);
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
