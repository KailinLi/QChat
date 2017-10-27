#ifndef RECEIVEFILETHREAD_H
#define RECEIVEFILETHREAD_H

#include <QObject>
#include "rdtreceiver.h"

class ReceiveFileThread : public QThread
{
    Q_OBJECT
public:
    ReceiveFileThread(QObject *parent, QFile *file,
                      QHostAddress &address, quint16 port,
                      QHostAddress &destination, quint16 destinationPort);
public:
    void run() Q_DECL_OVERRIDE;

    void stopListen();
    RdtReceiver *receiver;
private:
    volatile bool listening;
signals:
    void updateProgress(int);
};

#endif // RECEIVEFILETHREAD_H
