#include "rdtsender.h"
#define SENDSIZE 1380
#define N 130

RdtSender::RdtSender(QFile *file, QHostAddress &destination, quint16 destinationPort):
    receiver(new QUdpSocket()),
    baseSize(0),
    timer(new QTimer())
{
    timer->setInterval (200);

    connect (timer, &QTimer::timeout, this, &RdtSender::timeOut);

    sender = new RdtSenderSocket(nullptr, file, destination, destinationPort, file->size ());
    sender->moveToThread (&thread);
    connect (this, &RdtSender::sendFile, sender, &RdtSenderSocket::sendFile, Qt::QueuedConnection);
    connect (this, &RdtSender::resendFile, sender, &RdtSenderSocket::timeOut, Qt::QueuedConnection);
    connect (this, &RdtSender::deleteSender, sender, &RdtSenderSocket::deleteSelf, Qt::QueuedConnection);
//    connect (sender, &RdtSender::updateProgress, this, &SendFile::updateProcess, Qt::QueuedConnection);
    connect (&thread, &QThread::finished, &thread, &QThread::deleteLater);

//    connect (this, &SendFile::startSend, sender, &RdtSender::startSend, Qt::QueuedConnection);
//    connect (sender, &RdtSender::finish, this, &SendFile::finishSend, Qt::QueuedConnection);
    thread.start ();
}

RdtSender::~RdtSender()
{
    thread.quit ();
    thread.wait ();
    if (receiver) {
        receiver->deleteLater ();
    }
    if (timer) {
        timer->deleteLater ();
    }
    if (file) {
        file->close ();
        file->deleteLater ();
    }
}

void RdtSender::startSend()
{
    timer->start ();
    emit sendFile (&base, &nextSeqnum);

}


void RdtSender::readRdtACK()
{
    receiver->readDatagram (dataGram.data (), sizeof(qint64));
    QDataStream in(dataGram);
    in.setVersion (QDataStream::Qt_5_6);
    qint64 sequence;
    in >> sequence;
    base = sequence;
    if (base == nextSeqnum) {
        timer->start ();
    }
    else {
        timer->start ();
    }
    if (!(base % (SENDSIZE * 800)))
        emit updateProgress (base);
    if (base >= totalSize) {
        if (sender) {
            emit deleteSender ();
        }
        emit finish ();
    }
    else if (base > baseSize){
        baseSize += N * SENDSIZE;
        emit sendFile (&base, &nextSeqnum);
    }
}

void RdtSender::setFile(QFile *file)
{
    this->file = file;
    totalSize = file->size ();
    base = 0;
    nextSeqnum = 0;
}



void RdtSender::bindListen(QHostAddress &address, quint16 port)
{
    receiver->bind (address, port);
    connect (receiver, &QUdpSocket::readyRead, this, &RdtSender::readRdtACK);
    dataGram.resize (sizeof(qint64));
}

void RdtSender::timeOut()
{
    qDebug() << "time out";
    timer->start ();
    emit resendFile (&base, &nextSeqnum);
}

