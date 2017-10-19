#include "parallelserver.h"


ParallelServer::ParallelServer(QObject *parent):
    QTcpServer(parent)
{

}

void ParallelServer::incomingConnection(qintptr socketDescriptor)
{
    emit newConnection (socketDescriptor);
}
