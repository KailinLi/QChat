#include "receivefilethread.h"

ReceiveFileThread::ReceiveFileThread(QObject *parent):
    QThread(parent),
    receiver(new RdtReceiver(this))
{
    connect (receiver, &RdtReceiver::updateProgress, this, &ReceiveFileThread::updateProcess);
    connect (receiver, &RdtReceiver::finish, this, &ReceiveFileThread::stop);
    stopflag = true;
}

void ReceiveFileThread::run()
{
    while(stopflag);
}

void ReceiveFileThread::stop()
{
    stopflag = false;
}
