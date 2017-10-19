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
    void addThread(ConnectThread*);
    void removeThread(ConnectThread*);
    ConnectThread* findThread(quint32 ID);
    quint32 findID(ConnectThread*);
};

#endif // CONNECTTHREADPOOL_H
