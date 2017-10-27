#include "sendfilethread.h"

SendFileThread::SendFileThread(QObject *parent):
    QThread(parent)
{
    sender = new RdtSender(this);
    connect (sender, &RdtSender::updateProgress, this, &SendFileThread::updateProgress);
}

void SendFileThread::run()
{
    sender->setFile (file);
    sender->setAddress (destination);
    sender->setPort (destinationPort);
    sender->initThread (address, port);

    sender->startSend ();

    if (sender) {
        sender->close ();
        sender->deleteLater ();
    }
}

void SendFileThread::init(QFile *file, QHostAddress &destination, quint16 destinationPort,
                          QHostAddress &address, quint16 port)
{
    this->file = file;
    this->destination = destination;
    this->destinationPort = destinationPort;
    this->address = address;
    this->port = port;
}
