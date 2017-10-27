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

    qint64 sendSize;
    qint64 totalSize;
    qint64 bytesHadWritten;
    qint64 bytesNotWrite;

    QByteArray outBlock;

public:
    volatile RdtSenderThread::State state;
    void setAddress(QHostAddress address);
    void setPort(quint16 port);
    void setFile(QFile *file);

    void initThread(QHostAddress &address, quint16 port);
    void startSend();
signals:
    void updateProgress(qint64);
};

#endif // RDTSENDER_H
