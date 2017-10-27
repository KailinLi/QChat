#include "receivefilethread.h"

ReceiveFileThread::ReceiveFileThread(QObject *parent):
    QThread(parent),
    rcv(new RdtReceiver(this, QHostAddress("127.0.0.1"), 6000))
{
    connect (rcv, &RdtReceiver::step, this, &ReceiveFileThread::updateProcess);
    connect (rcv, &RdtReceiver::finish, this, &ReceiveFileThread::stop);
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
