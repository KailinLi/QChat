#include "sendfilethread.h"

SendFileThread::SendFileThread(QObject *parent):
    QThread(parent)
{

}

void SendFileThread::run()
{
    while (true) {}
}
