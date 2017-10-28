#include "rdtsender.h"

RdtSender::RdtSender(QObject *parent):
    QUdpSocket(parent),
    receiver(new QUdpSocket(this))
{
//    connect (this, &RdtSender::canSend, this, &RdtSender::sendFilePiece);
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

void RdtSender::startSend()
{
    while (bytesNotWrite > 0) {
        if (state == State::Send) {
            state = State::Wait;
//            emit canSend ();
            QDataStream stream(&outBlock, QIODevice::WriteOnly);
            stream.setVersion (QDataStream::Qt_5_6);
            stream << qMin(sendSize, bytesNotWrite);
            stream << bytesHadWritten;
            qint64 size = qMin(sendSize, bytesNotWrite);
            stream << file->read (size);
            writeDatagram (outBlock.constData (), outBlock.size (), destination, destinationPort);
            outBlock.resize (0);
            bytesNotWrite -= size;
            bytesHadWritten += size;
            emit updateProgress (bytesHadWritten);
        }
    }
    file->close ();
    file->deleteLater ();
    receiver->deleteLater ();
    deleteLater ();
    qDebug() << "ok";
}

//void RdtSender::sendFilePiece()
//{
//    QDataStream stream(&outBlock, QIODevice::WriteOnly);
//    stream.setVersion (QDataStream::Qt_5_6);
//    stream << qMin(sendSize, bytesNotWrite);
//    stream << bytesHadWritten;
//    qint64 size = qMin(sendSize, bytesNotWrite);
//    stream << file->read (size);
//    writeDatagram (outBlock.constData (), outBlock.size (), destination, destinationPort);
//    outBlock.resize (0);
//    bytesNotWrite -= size;
//    bytesHadWritten += size;
//    emit updateProgress (bytesHadWritten);

////    state = Wait;
////    QByteArray datagram;
////    qDebug() << tr("send %1").arg (current);
////    datagram.setNum (current++);
////    writeDatagram (datagram, destination, destinationPort);
//}

void RdtSender::readRdtACK()
{
//    QByteArray dataGram;
//    while (receiver->hasPendingDatagrams ()) {
//        dataGram.resize (receiver->pendingDatagramSize ());
//        receiver->readDatagram (dataGram.data (), dataGram.size ());
//        qDebug() << dataGram;
//    }
//    QString str = QString::fromUtf8(dataGram);
//    emit updateProgress(str.toInt ());
//    if (str.toInt () == 10000) state = Finish;
//    else state = Send;
    if (receiver->pendingDatagramSize ()< sizeof(qint64)) {
            return;
    }
//    dataGram.resize (sizeof(qint64));
    receiver->readDatagram (dataGram.data (), sizeof(qint64));
//    qint64 sequence = dataGram.toLongLong ();
//    qint64 number = qFromLittleEndian<qint64>(reinterpret_cast<const uchar *>(dataGram.data()));
//    qDebug() << number;
    state = State::Send;
}

void RdtSender::setFile(QFile *file)
{
    this->file = file;
    sendSize = 1200;
    totalSize = file->size ();
    bytesNotWrite = totalSize;
    bytesHadWritten = 0;
}

void RdtSender::setDestination(QHostAddress &destination, quint16 destinationPort)
{
    this->destination = destination;
    this->destinationPort = destinationPort;
//    this->data = 10;
//    this->current = 0;
}

void RdtSender::bindListen(QHostAddress &address, quint16 port)
{
    receiver->bind (address, port);
    connect (receiver, &QUdpSocket::readyRead, this, &RdtSender::readRdtACK);
    state = State::Send;
    dataGram.resize (sizeof(qint64));
}
