#ifndef QSERVER_H
#define QSERVER_H

#include <QWidget>
#include "connectthreadpool.h"
#include "parallelserver.h"
#include "userinfolist.h"

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
    UserInfoList userList;

    void msgSignIn(ConnectThread *thread, Message *msg);
    void msgSignUp(ConnectThread *thread, Message *msg);
    void msgForgetPassword(ConnectThread *thread, Message *msg);
    void msgLogIn(ConnectThread *thread, Message *msg);
    void msgFinishInit(ConnectThread *thread, Message *msg);
    void msgOfflineMsg(ConnectThread *thread, Message *msg);
private slots:
    void haveNewConnect(qintptr socketDescriptor);
    void haveNewMsg(ConnectThread* thread, Message* msg);
    void loseConnect(ConnectThread* thread);
signals:
    void msgToSend(ConnectThread *thread, Message *msg);
};

#endif // QSERVER_H
