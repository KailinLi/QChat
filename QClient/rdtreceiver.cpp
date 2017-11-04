#include "rdtreceiver.h"
#define SENDSIZE 1460

RdtReceiver::RdtReceiver(QObject *parent, QHostAddress destination, quint16 destinationPort):
    QUdpSocket(parent)
{
    updateTimer.setInterval (500);
    connect (&updateTimer, &QTimer::timeout, this, &RdtReceiver::canUpdate);
    sender = new RdtReceiverSocket(nullptr, destination, destinationPort);
    sender->moveToThread (&thread);
    connect (this, &RdtReceiver::sendACK, sender, &RdtReceiverSocket::sendACK, Qt::QueuedConnection);
    connect (this, &RdtReceiver::deleteSender, sender, &RdtReceiverSocket::deleteSelf);
    connect (&thread, &QThread::finished, &thread, &QThread::deleteLater);
    thread.start ();
}

RdtReceiver::~RdtReceiver()
{
    thread.quit ();
    thread.wait ();
    deleteLater ();
}

void RdtReceiver::readRdtData()
{

    dataGram.resize (static_cast<int>(pendingDatagramSize()));
    readDatagram (dataGram.data (), dataGram.size ());
    QDataStream in (dataGram);
    in.setVersion (QDataStream::Qt_5_6);
    in >> sequenceNumber >> blockSize;
    if (bytesHadWritten != sequenceNumber) {
        dataGram.resize (0);
    }
    else {
        bytesHadWritten += blockSize;
        file->write (dataGram.data () + (sizeof(qint64) + sizeof(qint32)), blockSize);
    }
    emit sendACK (bytesHadWritten);
    if (bytesHadWritten == totalSize) {
        qDebug() << "finish receive";
        updateTimer.stop ();
        file->close ();
        file->deleteLater ();
        qDebug() << "delete file";
        emit deleteSender ();

        qDebug() << "emit finish";
        emit finish ();
    }
}



void RdtReceiver::setFile(QFile *file, qint64 fileSize)
{
    this->file = file;
    this->totalSize = fileSize;
    this->blockSize = 0;
    this->bytesHadWritten = 0;
}



void RdtReceiver::bindListen(QHostAddress &address, quint16 port)
{
    bind (address, port);
    connect (this, &RdtReceiver::readyRead, this, &RdtReceiver::readRdtData);
    dataGram.resize (0);
    updateTimer.start ();
}

void RdtReceiver::canUpdate()
{
    emit updateProgress (bytesHadWritten);
}

