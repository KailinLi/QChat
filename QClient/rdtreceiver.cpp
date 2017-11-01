#include "rdtreceiver.h"
#define SendSize 1400

RdtReceiver::RdtReceiver(QObject *parent):
    QUdpSocket(parent),
    sender(new QUdpSocket(this))
{
    connect (this, &RdtReceiver::callACK, this, &RdtReceiver::sendACK);
}

void RdtReceiver::readRdtData()
{
    if (blockSize == 0) {
        if (pendingDatagramSize () + dataGram.size () < static_cast<qint64>(sizeof(qint64) * 2 + 4)) {
            return;
        }
        else {
            QByteArray appendBytes;
            appendBytes.resize (static_cast<int>(pendingDatagramSize ()));
            readDatagram (appendBytes.data (), appendBytes.size ());
            dataGram.append (appendBytes);
            QDataStream in(dataGram);
            in.setVersion (QDataStream::Qt_5_6);
            in >> blockSize >> sequenceNumber;
            dataGram.remove (0, sizeof(qint64) * 2 + 4);
        }
    }
    else {
        QByteArray appendBytes;
        appendBytes.resize (static_cast<int>(pendingDatagramSize ()));
        readDatagram (appendBytes.data (), appendBytes.size ());
        dataGram.append (appendBytes);
    }
    if (dataGram.size () < static_cast<qint64>(blockSize)) {
        return;
    }
    else {
        if (bytesHadWritten != sequenceNumber) {
//            dataGram.resize (0);//GBN
            qDebug() << tr("miss %1 %2").arg (bytesHadWritten).arg (sequenceNumber);
            dataGram.remove (0, blockSize);
            if (dataGram.size () >= sizeof(qint64) * 2 + 4) {
                QDataStream in(dataGram);
                in.setVersion (QDataStream::Qt_5_6);
                in >> blockSize >> sequenceNumber;
                dataGram.remove (0, sizeof(qint64) * 2 + 4);
            }
            else {
                blockSize = 0;
            }
        }
        else {
//            dataGram.resize (blockSize);
            bytesHadWritten += blockSize;
            file->write (dataGram, blockSize);
//            qDebug() << bytesHadWritten;
            dataGram.remove (0, blockSize);
            if (dataGram.size () >= sizeof(qint64) * 2 + 4) {
                QDataStream in(dataGram);
                in.setVersion (QDataStream::Qt_5_6);
                in >> blockSize >> sequenceNumber;
                dataGram.remove (0, sizeof(qint64) * 2 + 4);
            }
            else {
                blockSize = 0;
            }
        }
        emit callACK (bytesHadWritten);
        if (! (bytesHadWritten % (SendSize * 500)))
            emit updateProgress (bytesHadWritten);
        if (bytesHadWritten == totalSize) {
            qDebug() << "finish receive";
            file->close ();
            file->deleteLater ();
            qDebug() << "delete file";
            sender->disconnectFromHost ();
            sender->deleteLater ();
            qDebug() << "emit finish";
            emit finish ();
        }
//        blockSize = 0;
    }
}

void RdtReceiver::setFile(QFile *file, qint64 fileSize)
{
    this->file = file;
    this->totalSize = fileSize;
    this->blockSize = 0;
    this->bytesHadWritten = 0;
}

void RdtReceiver::setDestination(QHostAddress &destination, quint16 destinationPort)
{
    this->destination = destination;
    this->destinationPort = destinationPort;
    sender->connectToHost (destination, destinationPort);
}

void RdtReceiver::bindListen(QHostAddress &address, quint16 port)
{
    bind (address, port);
    connect (this, &RdtReceiver::readyRead, this, &RdtReceiver::readRdtData);
    dataGram.resize (0);
}

void RdtReceiver::sendACK(qint64 sequenceNumber)
{
    QDataStream stream(&block, QIODevice::WriteOnly);
    stream.setVersion (QDataStream::Qt_5_6);
    stream << sequenceNumber;
    qDebug() << sequenceNumber;
    sender->write (block.constData (), block.size ());
    block.resize (0);
}

