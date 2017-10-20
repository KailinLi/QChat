#ifndef QSERVER_H
#define QSERVER_H

#include <QWidget>
#include "connectthreadpool.h"
#include "parallelserver.h"

namespace Ui {
class QServer;
}

class QServer : public QWidget
{
    Q_OBJECT

public:
    explicit QServer(QWidget *parent = 0);
    ~QServer();

private:
    Ui::QServer *ui;
    ConnectThreadPool threadPool;
    ParallelServer *server;
private slots:
    void haveNewConnect(qintptr socketDescriptor);
    void haveNewMsg(ConnectThread* thread, Message* msg);
};

#endif // QSERVER_H
