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
    volatile bool receiving;
public:
    RdtReceiver *receiver;
signals:
    void updateProcess(qint64 t);
    void finishReceive();
};

#endif // RECEIVEFILETHREAD_H
