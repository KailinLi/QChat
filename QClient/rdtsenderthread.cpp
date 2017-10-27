#include "rdtsenderthread.h"

RdtSenderThread::RdtSenderThread(QObject *parent, QHostAddress &address, quint16 port, volatile State *state):
    QThread(parent),
    listening(true),
    state(state)
{
    receiver = new QUdpSocket(this);
    receiver->bind (address, port);
    connect (receiver, &QUdpSocket::readyRead, this, &RdtSenderThread::readData);
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

void RdtSenderThread::readData()
{
    if (receiver->bytesAvailable () < static_cast<qint64>(sizeof(quint64))) {
        return;
    }
    QByteArray dataGram;
    dataGram.resize (static_cast<qint64>(sizeof(quint64)));
    receiver->readDatagram (dataGram.data (), dataGram.size ());
    quint64 sequence = dataGram.toULongLong ();
    *state = State::Send;
}
