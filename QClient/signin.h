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

private:
    Ui::SignIn *ui;
    TcpSocketMsg* tcpMsg;

public:
    void tryConnect();
    void trySignIn();
};

#endif // SIGNIN_H
