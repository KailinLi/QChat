#ifndef SIGNUP_H
#define SIGNUP_H

#include <QDialog>
#include "tcpsocketmsg.h"

namespace Ui {
class SignUp;
}

class SignUp : public QDialog
{
    Q_OBJECT

public:
    explicit SignUp(QWidget *parent = 0);
    ~SignUp();

private:
    Ui::SignUp *ui;
    TcpSocketMsg *tcpMsg;

public:
    void tryConnect();
    void trySignUp();
    void haveNewMsgFromServer();
signals:
    void sendMsg(Message* msg);
private slots:
    void closeWindow();
};

#endif // SIGNUP_H
