#include "rdtreceiver.h"
#define SENDSIZE 1380

RdtReceiver::RdtReceiver(QObject *parent, QHostAddress destination, quint16 destinationPort):
    QUdpSocket(parent)
{

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
    in >> blockSize >> sequenceNumber;
    if (bytesHadWritten != sequenceNumber) {
        dataGram.resize (0);
    }
    else {
        bytesHadWritten += blockSize;
//        file->write (dataGram.data () + (sizeof(qint64) * 2 + 4), blockSize);
    }
    emit sendACK (bytesHadWritten);
    if (! (bytesHadWritten % (SENDSIZE * 800)))
        emit updateProgress (bytesHadWritten);
    if (bytesHadWritten == totalSize) {
        qDebug() << "finish receive";
        file->close ();
        file->deleteLater ();
        qDebug() << "delete file";
        emit deleteSender ();

        qDebug() << "emit finish";
        emit finish ();
    }
}





//    if (blockSize == 0) {
//        if (pendingDatagramSize () + dataGram.size () < static_cast<qint64>(sizeof(qint64) * 2 + 4)) {
//            qDebug() << "hehe";
//            return;
//        }
//        else {
//            QByteArray appendBytes;
//            appendBytes.resize (static_cast<int>(pendingDatagramSize ()));
//            readDatagram (appendBytes.data (), appendBytes.size ());
//            dataGram.append (appendBytes);
//            QDataStream in(dataGram);
//            in.setVersion (QDataStream::Qt_5_6);
//            in >> blockSize >> sequenceNumber;
//            dataGram.remove (0, sizeof(qint64) * 2 + 4);
//        }
//    }
//    else {
//        qDebug() << "append";
//        QByteArray appendBytes;
//        appendBytes.resize (static_cast<int>(pendingDatagramSize ()));
//        readDatagram (appendBytes.data (), appendBytes.size ());
//        dataGram.append (appendBytes);
//    }
//    if (dataGram.size () < static_cast<qint64>(blockSize)) {
//        qDebug() << "not enough";
//        return;
//    }
//    else {
//        if (bytesHadWritten != sequenceNumber) {
////            dataGram.resize (0);//GBN
//            qDebug() << tr("miss %1 %2").arg (bytesHadWritten).arg (sequenceNumber);
//            dataGram.remove (0, blockSize);
//            if (dataGram.size () >= sizeof(qint64) * 2 + 4) {
//                qDebug() << "read blockSize direct";
//                QDataStream in(dataGram);
//                in.setVersion (QDataStream::Qt_5_6);
//                in >> blockSize >> sequenceNumber;
//                dataGram.remove (0, sizeof(qint64) * 2 + 4);
//            }
//            else {
//                blockSize = 0;
//            }
//        }
//        else {
////            dataGram.resize (blockSize);
//            bytesHadWritten += blockSize;

//            file->write (dataGram, blockSize);

////            qDebug() << bytesHadWritten;
//            dataGram.remove (0, blockSize);
//            if (dataGram.size () >= sizeof(qint64) * 2 + 4) {
//                qDebug() << "read blockSize direct!!";
//                QDataStream in(dataGram);
//                in.setVersion (QDataStream::Qt_5_6);
//                in >> blockSize >> sequenceNumber;
//                dataGram.remove (0, sizeof(qint64) * 2 + 4);
//            }
//            else {
//                blockSize = 0;
//            }
//        }
//        emit sendACK (bytesHadWritten);
//        if (! (bytesHadWritten % (SENDSIZE * 800)))
//            emit updateProgress (bytesHadWritten);
//        if (bytesHadWritten == totalSize) {
//            qDebug() << "finish receive";
//            file->close ();
//            file->deleteLater ();
//            qDebug() << "delete file";
//            emit deleteSender ();

//            qDebug() << "emit finish";
//            emit finish ();
//        }
////        blockSize = 0;
//    }
//}

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
}

