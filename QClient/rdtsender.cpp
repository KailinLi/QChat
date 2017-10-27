#include "rdtsender.h"

RdtSender::RdtSender(QObject *parent):
    QUdpSocket(parent),
    sendSize(1200),
    totalSize(0),
    bytesHadWritten(0),
    bytesNotWrite(0)
{

}

void RdtSender::initThread(QHostAddress &address, quint16 port)
{
    totalSize = static_cast<quint64>(file->size ());
    bytesNotWrite = totalSize;
    thread = new RdtSenderThread(this, address, port, &state);
    connect (thread, &RdtSenderThread::finished, thread, &RdtSenderThread::deleteLater, Qt::QueuedConnection);
    thread->start ();
}

void RdtSender::startSend()
{
    while (bytesNotWrite > 0) {
        while (state == RdtSenderThread::State::Send) {
            QDataStream stream(&outBlock, QIODevice::WriteOnly);
            stream << qMin(sendSize, bytesNotWrite);
            stream << bytesHadWritten;
            stream << file->read (qMin(sendSize, bytesNotWrite));
            writeDatagram (outBlock.constData (), outBlock.size (), destination, destinationPort);
            state = RdtSenderThread::State::Wait;
        }
    }
}

void RdtSender::setAddress(QHostAddress address)
{
    destination = address;
}

void RdtSender::setPort(quint16 port)
{
    destinationPort = port;
}

void RdtSender::setFile(QFile *file)
{
    this->file = file;
}

