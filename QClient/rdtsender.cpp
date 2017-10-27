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
    state = RdtSenderThread::State::Send;
    thread = new RdtSenderThread(this);
    connect (thread, &RdtSenderThread::finished, thread, &RdtSenderThread::deleteLater, Qt::QueuedConnection);
    thread->init (address, port, &state);
    thread->start ();
}

void RdtSender::startSend()
{
    while (bytesNotWrite > 0) {
        while (state == RdtSenderThread::State::Send) {
            QDataStream stream(&outBlock, QIODevice::WriteOnly);
            stream.setVersion (QDataStream::Qt_5_6);
            stream << qMin(sendSize, bytesNotWrite);
            stream << bytesHadWritten;
            stream << file->read (qMin(sendSize, bytesNotWrite));
            writeDatagram (outBlock.constData (), outBlock.size (), destination, destinationPort);
            qDebug() << destinationPort;
            state = RdtSenderThread::State::Wait;
            outBlock.resize (0);
            bytesNotWrite -= qMin(sendSize, bytesNotWrite);
            bytesHadWritten += qMin(sendSize, bytesNotWrite);
            emit updateProgress (bytesHadWritten);
        }
    }
    file->close ();
    file->deleteLater ();
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

