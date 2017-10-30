#include "rdtsender.h"
#define sendSize 1300

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
            qint64 size = (sendSize > bytesNotWrite) ? bytesNotWrite : sendSize;//qMin(sendSize, bytesNotWrite);
            stream << size;
            stream << bytesHadWritten;
//            qint64 size = qMin(sendSize, bytesNotWrite);
            stream << file->read (size);
            write (outBlock.constData (), outBlock.size ());
            outBlock.resize (0);
            bytesNotWrite -= size;
            bytesHadWritten += size;
            if (!(bytesHadWritten % (sendSize * 200)))
                emit updateProgress (bytesHadWritten);
        }
    }
    qDebug() << "startSend finish";
    file->close ();
    file->deleteLater ();
    qDebug() << "delete file";
    receiver->deleteLater ();
    qDebug() << "delete receiver";
//    disconnectFromHost ();
//    deleteLater ();
    qDebug() << "return startSend";
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
    totalSize = file->size ();
    bytesNotWrite = totalSize;
    bytesHadWritten = 0;
}

void RdtSender::setDestination(QHostAddress &destination, quint16 destinationPort)
{
    this->destination = destination;
    this->destinationPort = destinationPort;
    connectToHost (destination, destinationPort);
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
