#ifndef TCPSOCKETMSG_H
#define TCPSOCKETMSG_H

#include <QObject>
#include <QtNetwork>

class Message;

class TcpSocketMsg : public QTcpSocket
{
    Q_OBJECT
public:
    TcpSocketMsg(QObject* parent);
    qint64 blockSize;
    void send(Message& msg);
    Message *read();
    //return nullptr if not ready
    //Don't forget delete the Message!!!
};

#endif // TCPSOCKETMSG_H
