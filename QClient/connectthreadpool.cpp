#include "connectthreadpool.h"

ConnectThreadPool::ConnectThreadPool()
{

}

void ConnectThreadPool::addThread(ConnectThread *thread)
{
    threadPool.push_back (thread);
    thread->start ();
}

void ConnectThreadPool::removeThread(ConnectThread *thread)
{
    thread->stop ();
    threadPool.removeOne (thread);
    qDebug() << "delete one thread";
}

ConnectThread *ConnectThreadPool::getThread(quint32 ID)
{
    foreach (ConnectThread* thread, threadPool) {
        if (thread->getUserID ()== ID ) return thread;
    }
    return nullptr;
}

quint32 ConnectThreadPool::getID(ConnectThread *thread)
{
    return thread->getUserID ();
}

void ConnectThreadPool::closeAll()
{
    foreach (ConnectThread *thread, threadPool) {
        removeThread (thread);
    }
}
