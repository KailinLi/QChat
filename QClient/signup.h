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

    quint32 *id;
    QHostAddress *serverAddress;

private:
    Ui::SignUp *ui;
    TcpSocketMsg *tcpMsg;

public:
    void tryConnect();
    void trySignUp();
    void haveNewMsgFromServer();
    void clickSetBtn();
    void shakeWindow();
signals:
    void sendMsg(Message* msg);
private slots:
    void closeWindow();
};

#endif // SIGNUP_H
