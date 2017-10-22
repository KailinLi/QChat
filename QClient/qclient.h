#ifndef QCLIENT_H
#define QCLIENT_H

#include <QWidget>
#include "connectthreadpool.h"
#include "parallelserver.h"
#include "userinfolist.h"

namespace Ui {
class QClient;
}

class QClient : public QWidget
{
    Q_OBJECT

public:
    explicit QClient(QWidget *parent = 0);
    ~QClient();

    quint32 userID;

private:
    Ui::QClient *ui;
    ParallelServer *server;
    ConnectThreadPool threadPool;
    UserInfoList userList;
    TcpSocketMsg* tcpToServer;
    QString userName;
    quint32 currentID;
public:
    void start();
    void logIn();
    void haveNewMsgFromServer();
    void initUpdateUI();
    void signInUpdateUI(const QString &name);
    void signOutUpdateUI(const QString &name);
    void quit();
    void makeActive(int row);
public slots:

signals:
    void sendMsg(Message* msg);
};

#endif // QCLIENT_H
