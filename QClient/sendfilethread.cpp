#include "sendfilethread.h"

SendFileThread::SendFileThread(QObject* parent):
    QThread(parent),
    sender(new RdtSender()),
    sending(true)
{
//    sender = new RdtSender();
//    sender->setDestination (QHostAddress("127.0.0.1"), 6001);
//    sender->bindListen (QHostAddress("127.0.0.1"), 6000);
    connect (sender, &RdtSender::updateProgress, this, &SendFileThread::updateProcess);
//    connect (sender, &RdtSender::disconnected, sender, &RdtSender::deleteLater);
}

void SendFileThread::run()
{
    sender->startSend ();
    qDebug() << "getBack";
    if (sender) {
        sender->disconnectFromHost ();
        sender->deleteLater ();
    }
    emit finishSend ();
    while (sending) {}
    qDebug() << "will finish run";
}

void SendFileThread::stop()
{
    sending = false;
}
