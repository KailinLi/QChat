#include "rdtreceiver.h"

RdtReceiver::RdtReceiver(QObject *parent):
    QUdpSocket(parent),
    blockSize(0)
{
    connect (this, &RdtReceiver::readyRead, this, &RdtReceiver::readRdtData);
}

void RdtReceiver::setFile(QFile *file)
{
    this->file = file;
}

void RdtReceiver::setTotalSize(qint64 fileSize)
{
    totalSize = fileSize;
}

void RdtReceiver::initThread(QHostAddress &destination, quint16 destinationPort)
{
    thread =  new RdtReceiverThread(this, destination, destinationPort);
    connect (thread, &RdtReceiverThread::finished, thread, &RdtReceiverThread::deleteLater);
    connect (this, &RdtReceiver::sendACK, thread, &RdtReceiverThread::sendACK);
    thread->start ();
}

void RdtReceiver::readRdtData()
{
    if (blockSize == 0) {
        if (bytesAvailable () < static_cast<qint64>(sizeof(qint64) * 2)) {
            return;
        }
        else {
            QByteArray dataGram;
            dataGram.resize (static_cast<int>(sizeof(qint64) * 2));
            readDatagram (dataGram.data (), dataGram.size ());
            QDataStream in(dataGram);
            in.setVersion (QDataStream::Qt_5_6);
            in >> blockSize >> sequenceNumber;
        }
    }
    if (bytesAvailable () < static_cast<qint64>(blockSize)) {
        return;
    }
    else {
        QByteArray dataGram;
        dataGram.resize (static_cast<int>(blockSize));
        readDatagram (dataGram.data (), dataGram.size ());
        if (bytesHadWritten != sequenceNumber) {
            dataGram.resize (0);//GBN
        }
        else {
            bytesHadWritten += blockSize;
            file->write (dataGram);
        }
        emit sendACK (bytesHadWritten);
        emit updateProgress (bytesHadWritten);
        if (bytesHadWritten == totalSize) {
            file->close ();
            file->deleteLater ();
        }
        blockSize = 0;
    }
}
