#ifndef CONNECTTHREADPOOL_H
#define CONNECTTHREADPOOL_H

#include <QObject>
#include "connectthread.h"


class ConnectThreadPool
{
public:
    ConnectThreadPool();
private:
    QList<ConnectThread*> threadPool;
public:
    void addThread(ConnectThread* thread);
    void removeThread(ConnectThread* thread);
    ConnectThread *getThread(quint32 ID);
    quint32 getID(ConnectThread* thread);
};

#endif // CONNECTTHREADPOOL_H
