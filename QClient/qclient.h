#ifndef QCLIENT_H
#define QCLIENT_H

#include <QWidget>
#include "connectthreadpool.h"
#include "parallelserver.h"

namespace Ui {
class QClient;
}

class QClient : public QWidget
{
    Q_OBJECT

public:
    explicit QClient(QWidget *parent = 0);
    ~QClient();

private:
    Ui::QClient *ui;
    quint32 userID;
    ParallelServer *server;
    ConnectThreadPool threadPool;
};

#endif // QCLIENT_H
