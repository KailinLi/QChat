#ifndef RDTSENDERTHREAD_H
#define RDTSENDERTHREAD_H

#include <QObject>
#include <QThread>
#include <QtNetwork>

class RdtSenderThread : public QThread
{
    Q_OBJECT
public:

    enum State : int {
        Send,
        Wait
    };
    RdtSenderThread(QObject *parent);

public:
    void run() Q_DECL_OVERRIDE;
    void init (QHostAddress &address, quint16 port, volatile RdtSenderThread::State *state);
    volatile bool listening;
    QUdpSocket *receiver;
    volatile State *state;
    void stopListen();
    void readRdtData();

private:
    QByteArray block;
};

#endif // RDTSENDERTHREAD_H
