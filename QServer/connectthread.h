#ifndef CONNECTTHREAD_H
#define CONNECTTHREAD_H

#include <QObject>
#include "tcpsocketmsg.h"

class ConnectThread : public QThread
{
    Q_OBJECT
public:
    ConnectThread(int socketDescriptor, QObject* parent);

private:
    int socketDescriptor;
    volatile bool runThread;
    quint32 userID;
public:
    void run() Q_DECL_OVERRIDE;
    void stop();
    void setUserID(quint32 id);
    quint32 getUserID();
    TcpSocketMsg *tcpMsg;

signals:
    void newMsg(Message*);
    void error(TcpSocketMsg::SocketError socketError);
    void loseConnect(ConnectThread*);
private slots:
    void threadReadyRead();
    void disconnect();
};

#endif // CONNECTTHREAD_H
