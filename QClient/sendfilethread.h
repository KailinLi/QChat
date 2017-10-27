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
private:
    RdtSender *sender;
signals:
    void updateProcess(qint64 x);
};

#endif // SENDFILETHREAD_H
