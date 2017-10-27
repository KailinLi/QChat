#ifndef RDTSENDER_H
#define RDTSENDER_H

#include <QObject>
#include "rdtsenderthread.h"
#include <QFile>

class RdtSender : public QUdpSocket
{
    Q_OBJECT
public:
    RdtSender(QObject *parent);

private:
    QHostAddress destination;
    quint16 destinationPort;
    QFile *file;
    RdtSenderThread *thread;

    quint64 sendSize;
    quint64 totalSize;
    quint64 bytesHadWritten;
    quint64 bytesNotWrite;

    QByteArray outBlock;

public:
    volatile RdtSenderThread::State state;
    void setAddress(QHostAddress address);
    void setPort(quint16 port);
    void setFile(QFile *file);

    void initThread(QHostAddress &address, quint16 port);
    void startSend();
};

#endif // RDTSENDER_H
