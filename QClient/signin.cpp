#include "signin.h"
#include "ui_signin.h"

SignIn::SignIn(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SignIn),
    tcpMsg(new TcpSocketMsg(this))
{
    ui->setupUi(this);
    connect (ui->signinBtn, &QPushButton::clicked, this, &SignIn::tryConnect);
    connect (tcpMsg, &TcpSocketMsg::connected, this, &SignIn::trySignIn);
//    connect (tcpMsg, &TcpSocketMsg::readyRead, this, )
}

SignIn::~SignIn()
{
    delete ui;
}

void SignIn::tryConnect()
{
    tcpMsg->setBlockSize (0);
    if (tcpMsg) tcpMsg->abort ();
    tcpMsg->connectToHost (QHostAddress(tr("10.11.57.240")), 6666);
}

void SignIn::trySignIn()
{
    Message *msg = new Message(Message::SignIn);
    msg->addArgv(ui->nameLineEdit->text ());
    msg->addArgv(ui->passwdLineEdit->text ());
    tcpMsg->send (msg);
}
