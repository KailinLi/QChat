#ifndef TCPSOCKETMSG_H
#define TCPSOCKETMSG_H

#include <QObject>
#include <QtNetwork>
#include <QDebug>
#include "message.h"


class TcpSocketMsg : public QTcpSocket
{
    Q_OBJECT
private:
    qint64 blockSize;
public:
    TcpSocketMsg(QObject* parent);
    void setBlockSize(qint64 i);
    void send(Message* msg);
    Message *read();
    //return nullptr if not ready
    //Don't forget delete the Message!!!
};

#endif // TCPSOCKETMSG_H
