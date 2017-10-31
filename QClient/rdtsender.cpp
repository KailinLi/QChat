#include "rdtsender.h"
#define SendSize 1400
#define N 150

RdtSender::RdtSender(QObject *parent):
    QUdpSocket(parent),
    receiver(new QUdpSocket(this))
{
//    connect (this, &RdtSender::canSend, this, &RdtSender::sendFilePiece);
    connect (&timer, &QTimer::timeout, this, &RdtSender::timeOut);
}

void RdtSender::startSend()
{
    while (base < totalSize) {
        if (nextSeqnum < base + N * SendSize && nextSeqnum < totalSize) {
//        if (state == State::Send) {
//            state = State::Wait;
            QDataStream stream(&outBlock, QIODevice::WriteOnly);
            stream.setVersion (QDataStream::Qt_5_6);
            qint64 size = (SendSize > (totalSize - nextSeqnum)) ? (totalSize - nextSeqnum) : SendSize;//qMin(sendSize, bytesNotWrite);
            stream << size;
            stream << nextSeqnum;
            stream << file->read (size);
            write (outBlock.constData (), outBlock.size ());
            outBlock.resize (0);

            if (base == nextSeqnum) timer.start ();

            nextSeqnum += size;
//            bytesNotWrite -= size;
//            base += size;
//            if (!(bytesHadWritten % (sendSize * 200)))
//                emit updateProgress (bytesHadWritten);
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


void RdtSender::readRdtACK()
{
    if (receiver->pendingDatagramSize ()< sizeof(qint64)) {
            return;
    }
//    dataGram.resize (sizeof(qint64));
    receiver->readDatagram (dataGram.data (), sizeof(qint64));
    QDataStream in(dataGram);
    in.setVersion (QDataStream::Qt_5_6);
    qint64 sequence;
    in >> sequence;
    base = sequence;
    if (base == nextSeqnum) {
        timer.stop ();
    }
    else {
        timer.start ();
    }
    if (!(base % (SendSize * 300)))
        emit updateProgress (base);
//    bytesNotWrite -= sequence;
//    qint64 sequence = dataGram.toLongLong ();
//    qint64 number = qFromLittleEndian<qint64>(reinterpret_cast<const uchar *>(dataGram.data()));
//    qDebug() << number;
//    state = State::Send;
}

void RdtSender::setFile(QFile *file)
{
    this->file = file;
    totalSize = file->size ();
//    bytesNotWrite = totalSize;
    base = 0;
    nextSeqnum = 0;
}

void RdtSender::setDestination(QHostAddress &destination, quint16 destinationPort)
{
    this->destination = destination;
    this->destinationPort = destinationPort;
    connectToHost (destination, destinationPort);
}

void RdtSender::bindListen(QHostAddress &address, quint16 port)
{
    receiver->bind (address, port);
    connect (receiver, &QUdpSocket::readyRead, this, &RdtSender::readRdtACK);
    state = State::Send;
    dataGram.resize (sizeof(qint64));
    timer.setInterval (1000);
}

void RdtSender::timeOut()
{
    qDebug() << "time out";
    timer.start ();
    file->seek (base);
    for (qint64 index = base; index < nextSeqnum; index += SendSize) {
        QDataStream stream(&outBlock, QIODevice::WriteOnly);
        stream.setVersion (QDataStream::Qt_5_6);
        qint64 size = (SendSize > (totalSize - index)) ? (totalSize - index) : SendSize;//qMin(sendSize, bytesNotWrite);
        stream << size;
        stream << index;
        stream << file->read (size);
        write (outBlock.constData (), outBlock.size ());
        outBlock.resize (0);
        index += size;
    }
}
