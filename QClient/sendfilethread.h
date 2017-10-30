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
signals:
    void updateProcess(qint64 x);
    void finishSend();
};

#endif // SENDFILETHREAD_H
