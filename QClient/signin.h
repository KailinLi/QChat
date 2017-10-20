#ifndef SIGNIN_H
#define SIGNIN_H

#include <QDialog>
#include "tcpsocketmsg.h"

namespace Ui {
class SignIn;
}

class SignIn : public QDialog
{
    Q_OBJECT

public:
    explicit SignIn(QWidget *parent = 0);
    ~SignIn();
    enum DoneType : int {
        SignInSuccess = 1,
        Exit = 2,
        SignUp = 3,
        GetPassword = 4
    };

private:
    Ui::SignIn *ui;
    TcpSocketMsg* tcpMsg;

public:
    void tryConnect();
    void trySignIn();
    void haveNewMsgFromServer();
signals:
    void sendMsg(Message* msg);
private slots:
    void closeWindow();
};

#endif // SIGNIN_H
