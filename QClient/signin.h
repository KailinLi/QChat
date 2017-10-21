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
        SignUp = 3
//        GetPassword = 4
    };
    quint32 *id;

private:
    Ui::SignIn *ui;
    TcpSocketMsg* tcpMsg;
    bool needPassword;

public:
    void tryConnect();
    void trySignIn();
    void haveNewMsgFromServer();
    void tryFindPassword();
    bool showFindPasswordDialog(const QString& pwQuestion, const QString& pwAnswer);
signals:
    void sendMsg(Message* msg);
private slots:
    void callSignUp();
    void closeWindow();
    void forgetPassword();
};

#endif // SIGNIN_H
