#include "sendfilethread.h"

SendFileThread::SendFileThread(QObject *parent, QFile *file,
                               QHostAddress &destination, quint16 destinationPort,
                               QHostAddress &address, quint16 port):
    QThread(parent)
{
    sender = new RdtSender(this);
    sender->setFile (file);
    sender->setAddress (destination);
    sender->setPort (destinationPort);
    sender->initThread (address, port);
}

void SendFileThread::run()
{
    sender->startSend ();
    if (sender) {
        sender->close ();
        sender->deleteLater ();
    }
}
