#ifndef SENDFILETHREAD_H
#define SENDFILETHREAD_H

#include <QObject>
#include <QThread>
#include "rdtsender.h"

class SendFileThread : public QThread
{
    Q_OBJECT
public:
    SendFileThread(QObject* parent);
    void run() Q_DECL_OVERRIDE;
    void stop();
public:
    RdtSender *sender;
    volatile bool sending;
    void setFile(QFile *file);
    void setDestination(QHostAddress &destination, quint16 destinationPort);
    void setAddress(QHostAddress &address, quint16 port);
private:
    QFile *file;
    QHostAddress address;
    quint16 port;
    QHostAddress destination;
    quint16 destinationPort;
signals:
    void updateProcess(qint64 x);
    void finishSend();
};

#endif // SENDFILETHREAD_H
