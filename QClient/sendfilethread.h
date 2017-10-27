#ifndef SENDFILETHREAD_H
#define SENDFILETHREAD_H

#include <QObject>
#include "rdtsender.h"

class SendFileThread : public QThread
{
    Q_OBJECT
public:
    SendFileThread(QObject* parent, QFile *file,
                   QHostAddress &destination, quint16 destinationPort,
                   QHostAddress &address, quint16 port);

public:
    void run() Q_DECL_OVERRIDE;

    RdtSender *sender;
private:

signals:
    void updateProgress(int);
};

#endif // SENDFILETHREAD_H
