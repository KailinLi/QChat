#include "tcpsocketmsg.h"

TcpSocketMsg::TcpSocketMsg(QObject *parent):
    QTcpSocket(parent), blockSize(0)
{

}

void TcpSocketMsg::setBlockSize(qint64 i)
{
    blockSize = i;
}

void TcpSocketMsg::send(Message &msg)
{
    QByteArray message;
    QDataStream out(&message, QIODevice::WriteOnly);
    out.setVersion (QDataStream::Qt_5_6);
    out << (quint64)0;
    out << msg.getType ();
    out << msg.argc ();
    for (int i = 0; i < msg.argc (); ++i) {
        out << msg.getArgv (i);
    }
    out.device ()->seek (0);
    out << (quint64)(message.size () - sizeof(quint64));
    write (message);
}

Message *TcpSocketMsg::read()
{
    QDataStream in(this);
    in.setVersion (QDataStream::Qt_5_6);
    if (blockSize == 0) {
        if (bytesAvailable () < (qint64)sizeof(quint16)) return nullptr;
        in >> blockSize;
    }
    if (bytesAvailable () < blockSize) return nullptr;
    int t;
    in >> t;
    Message* msg = new Message(static_cast<Message::Type>(t));
    int argc;
    in >> argc;
    QString s;
    for (int i = 0; i < argc; ++i) {
        in >> s;
        msg->addArgv (s);
    }
    blockSize = 0;
    return msg;
}

