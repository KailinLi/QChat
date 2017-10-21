#ifndef PARALLELSERVER_H
#define PARALLELSERVER_H

#include <QObject>
#include <QtNetwork>

class ParallelServer : public QTcpServer
{
    Q_OBJECT
public:
    ParallelServer(QObject* parent);
protected:
    void incomingConnection(qintptr socketDescriptor) Q_DECL_OVERRIDE;
signals:
    void newConnection(qintptr);
};

#endif // PARALLELSERVER_H
