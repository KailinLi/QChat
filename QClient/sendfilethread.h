#ifndef SENDFILETHREAD_H
#define SENDFILETHREAD_H

#include <QObject>
#include "rdtsender.h"

class SendFileThread : public QThread
{
    Q_OBJECT
public:
    SendFileThread(QObject* parent);

public:
    void run() Q_DECL_OVERRIDE;
    void init(QFile *file,
              QHostAddress &destination, quint16 destinationPort,
              QHostAddress &address, quint16 port);
    RdtSender *sender;
private:
    QFile *file;
    QHostAddress destination;
    quint16 destinationPort;
    QHostAddress address;
    quint16 port;
signals:
    void updateProgress(qint64);
};

#endif // SENDFILETHREAD_H
