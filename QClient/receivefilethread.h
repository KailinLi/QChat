#ifndef RECEIVEFILETHREAD_H
#define RECEIVEFILETHREAD_H

#include <QObject>
#include "rdtreceiver.h"

class ReceiveFileThread : public QThread
{
    Q_OBJECT
public:
    ReceiveFileThread(QObject *parent);
public:
    void run() Q_DECL_OVERRIDE;
    void init (QFile *file,
              QHostAddress &address, quint16 port,
              QHostAddress &destination, quint16 destinationPort,
              qint64 fileSize);
    void stopListen();
    RdtReceiver *receiver;
private:
    volatile bool listening;
    QFile *file;
    QHostAddress address;
    quint16 port;
    QHostAddress destination;
    quint16 destinationPort;
    qint64 fileSize;
signals:
    void updateProgress(qint64);
};

#endif // RECEIVEFILETHREAD_H
