#include "rdtsendersocket.h"
#define N 130
#define SENDSIZE 1380

RdtSenderSocket::RdtSenderSocket(QObject *parent, QFile *file,
                                 QHostAddress destination, quint16 destinationPort, qint64 totalSize):
    QUdpSocket(parent),
    file(file),
    totalSize(totalSize)
{
    connectToHost (destination, destinationPort);
    outBlock.resize (0);
}

void RdtSenderSocket::sendFile(volatile qint64 *base, volatile qint64 *nextSeqnum)
{
    while (*nextSeqnum < *base + N * SENDSIZE && *nextSeqnum < totalSize) {
        QDataStream stream(&outBlock, QIODevice::WriteOnly);
        stream.setVersion (QDataStream::Qt_5_6);
        qint64 size = (SENDSIZE > (totalSize - *nextSeqnum)) ? (totalSize - *nextSeqnum) : SENDSIZE;//qMin(sendSize, bytesNotWrite);
        stream << size;
        stream << *nextSeqnum;
        stream << file->read (size);
        write (outBlock.constData (), outBlock.size ());
        outBlock.resize (0);

        *nextSeqnum += size;

    }
}

void RdtSenderSocket::timeOut(volatile qint64 *base, volatile qint64 *nextSeqnum)
{
    qDebug() << "call timeout";
    file->seek (*base);
    for (qint64 index = *base; index < *nextSeqnum; index += SENDSIZE) {
        QDataStream stream(&outBlock, QIODevice::WriteOnly);
        stream.setVersion (QDataStream::Qt_5_6);
        qint64 size = (SENDSIZE > (totalSize - index)) ? (totalSize - index) : SENDSIZE;//qMin(sendSize, bytesNotWrite);
        stream << size;
        stream << index;
        stream << file->read (size);
        write (outBlock.constData (), outBlock.size ());
        outBlock.resize (0);
        index += size;
    }
}

void RdtSenderSocket::deleteSelf()
{
    disconnectFromHost ();
    deleteLater ();
}
