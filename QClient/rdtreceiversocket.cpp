#include "rdtreceiversocket.h"

RdtReceiverSocket::RdtReceiverSocket(QObject *parent, QHostAddress destination, quint16 destinationPort):
    QUdpSocket(parent)
{
    block.resize (0);
    connectToHost (destination, destinationPort);
}

RdtReceiverSocket::~RdtReceiverSocket()
{
    disconnectFromHost ();
    deleteLater ();
//    qDebug() << "socketDelete";
}

void RdtReceiverSocket::sendACK(qint64 sequenceNumber)
{
    QDataStream stream(&block, QIODevice::WriteOnly);
    stream.setVersion (QDataStream::Qt_5_6);
    stream << sequenceNumber;
    write (block.constData (), block.size ());
    block.resize (0);
}

void RdtReceiverSocket::deleteSelf()
{
    this->~RdtReceiverSocket ();
}
