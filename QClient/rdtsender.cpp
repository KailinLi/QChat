#include "rdtsender.h"
#define SENDSIZE 1400
#define N 160

RdtSender::RdtSender(QFile *file, QHostAddress &destination, quint16 destinationPort):
    receiver(new QUdpSocket()),
    baseSize(0),
    timer(new QTimer())
//    timer(new QTimer())
{
//    connect (this, &RdtSender::timerStartSignal, this, &RdtSender::startTheTimer);
//    connect (this, &RdtSender::timerStopSignal, timer, &QTimer::stop);
//    connect (timer, &QTimer::timeout, this, &RdtSender::timeOut);
//    timer->setInterval (1000);

//    receiver = new QUdpSocket();
//    timer = new QTimer();
    timer->setInterval (1000);

    connect (timer, &QTimer::timeout, this, &RdtSender::timeOut);

    sender = new RdtSenderSocket(nullptr, file, destination, destinationPort, file->size ());
    sender->moveToThread (&thread);
    connect (this, &RdtSender::sendFile, sender, &RdtSenderSocket::sendFile, Qt::QueuedConnection);
    connect (this, &RdtSender::resendFile, sender, &RdtSenderSocket::timeOut, Qt::QueuedConnection);
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
    if (sender) {
        sender->disconnectFromHost ();
        sender->deleteLater ();
    }
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
//    qDebug() << QThread::currentThreadId ();
//    connectToHost (destination, destinationPort);
//    receiver = new QUdpSocket();
//    timer = new QTimer();
//    receiver->bind (address, port);
//    qDebug() << port;
//    connect (this, &RdtSender::timerStartSignal, this, &RdtSender::startTheTimer);
//    connect (this, &RdtSender::timerStopSignal, timer, &QTimer::stop);
//    connect (timer, &QTimer::timeout, this, &RdtSender::timeOut);
//    connect (receiver, &QUdpSocket::readyRead, this, &RdtSender::readRdtACK);

//    timer->setInterval (1000);


//    while (base < totalSize) {
//        if (nextSeqnum < base + N * SendSize && nextSeqnum < totalSize) {

//            QDataStream stream(&outBlock, QIODevice::WriteOnly);
//            stream.setVersion (QDataStream::Qt_5_6);
//            qint64 size = (SendSize > (totalSize - nextSeqnum)) ? (totalSize - nextSeqnum) : SendSize;//qMin(sendSize, bytesNotWrite);
//            stream << size;
//            stream << nextSeqnum;
//            stream << file->read (size);
//            write (outBlock.constData (), outBlock.size ());
//            outBlock.resize (0);

//            if (base == nextSeqnum) {
//                emit timerStartSignal ();
//            }
//                //timer->start ();

//            nextSeqnum += size;
//        }
//    }
//    qDebug() << "startSend finish";
//    file->close ();
//    file->deleteLater ();
//    qDebug() << "delete file";
//    receiver->deleteLater ();
//    qDebug() << "delete receiver";

//    qDebug() << "return startSend";
}


void RdtSender::readRdtACK()
{
//    qDebug() << "hello";
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
//        emit timerStopSignal ();
        timer->start ();
    }
    else {
//        emit timerStartSignal ();
        timer->start ();
    }
    if (!(base % (SENDSIZE * 300)))
//        qDebug() << base;
        emit updateProgress (base);
//    qDebug() << base << " " << baseSize;
    if (base >= totalSize) {
        emit finish ();
    }
    else if (base > baseSize){
        baseSize += (N / 2) * SENDSIZE;
        emit sendFile (&base, &nextSeqnum);
    }
//    else if ((base / SENDSIZE) % 2) {
//        qDebug() << "send";
//        emit sendFile (&base, &nextSeqnum);
//    }
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

//void RdtSender::setDestination(QHostAddress &destination, quint16 destinationPort)
//{
//    this->destination = destination;
//    this->destinationPort = destinationPort;
////    connectToHost (destination, destinationPort);
//}

void RdtSender::bindListen(QHostAddress &address, quint16 port)
{
//    receiver->bind (address, port);
//    connect (receiver, &QUdpSocket::readyRead, this, &RdtSender::readRdtACK);
//    this->address = address;
//    this->port = port;
    receiver->bind (address, port);
    connect (receiver, &QUdpSocket::readyRead, this, &RdtSender::readRdtACK);
    dataGram.resize (sizeof(qint64));
}

void RdtSender::timeOut()
{
    qDebug() << "time out";
    timer->start ();
    emit resendFile (&base, &nextSeqnum);
//    file->seek (base);
//    for (qint64 index = base; index < nextSeqnum; index += SendSize) {
//        QDataStream stream(&outBlock, QIODevice::WriteOnly);
//        stream.setVersion (QDataStream::Qt_5_6);
//        qint64 size = (SendSize > (totalSize - index)) ? (totalSize - index) : SendSize;//qMin(sendSize, bytesNotWrite);
//        stream << size;
//        stream << index;
//        stream << file->read (size);
//        write (outBlock.constData (), outBlock.size ());
//        outBlock.resize (0);
//        index += size;
//    }
}

//void RdtSender::startTheTimer()
//{
//    timer->start ();
//}
