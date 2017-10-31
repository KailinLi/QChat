#ifndef RECEIVEFILETHREAD_H
#define RECEIVEFILETHREAD_H

#include <QObject>
#include <QThread>
#include "rdtreceiver.h"

class ReceiveFileThread : public QThread
{
    Q_OBJECT
public:
    ReceiveFileThread(QObject *parent);
    void run () Q_DECL_OVERRIDE;
    void stop();
private:
//    volatile bool receiving;
    QFile *file;
    qint64 fileSize;
    QHostAddress destination;
    quint16 destinationPort;
    QHostAddress address;
    quint16 port;
public:
    RdtReceiver *receiver;
    void setFile(QFile *file, qint64 fileSize);
    void setDestination(QHostAddress &destination, quint16 destinationPort);
    void setAddress(QHostAddress &address, quint16 port);
signals:
    void updateProcess(qint64 t);
    void finishReceive();
};

#endif // RECEIVEFILETHREAD_H
