#include "sendfilethread.h"

SendFileThread::SendFileThread(QObject* parent):
    QThread(parent)
{
    sender = new RdtSender(this);
    sender->setSender (QHostAddress("127.0.0.1"), 6001);
    sender->setRcv (QHostAddress("127.0.0.1"), 6000);
    connect (sender, &RdtSender::step, this, &SendFileThread::updateProcess);
}

void SendFileThread::run()
{
    sender->sendData ();
}
