#include "receivefilethread.h"

ReceiveFileThread::ReceiveFileThread(QObject *parent):
    QThread(parent),
    receiving(true),
    receiver(new RdtReceiver())
{
    connect (receiver, &RdtReceiver::updateProgress, this, &ReceiveFileThread::updateProcess);
    connect (receiver, &RdtReceiver::finish, this, &ReceiveFileThread::finishReceive);
}

void ReceiveFileThread::run()
{
    while(receiving);
    receiver->deleteLater ();
    qDebug() << "finish receiver";
}

void ReceiveFileThread::stop()
{
    receiving = false;
}
