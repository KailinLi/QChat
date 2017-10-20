#include "signin.h"
#include "ui_signin.h"

SignIn::SignIn(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SignIn),
    tcpMsg(new TcpSocketMsg(this))
{
    ui->setupUi(this);
    ui->passwdLineEdit->setEchoMode (QLineEdit::Password);
    connect (ui->signinBtn, &QPushButton::clicked, this, &SignIn::tryConnect);
    connect (tcpMsg, &TcpSocketMsg::connected, this, &SignIn::trySignIn);
    connect (this, &SignIn::sendMsg, tcpMsg, &TcpSocketMsg::send);
    ui->nameLineEdit->setFocus ();
    ui->signinBtn->setAutoDefault (true);
    ui->signinBtn->setDefault (true);
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
    tcpMsg->connectToHost (QHostAddress(tr("127.0.0.1")), 6666);
}

void SignIn::trySignIn()
{
    Message *msg = new Message(Message::SignIn);
    msg->addArgv(ui->nameLineEdit->text ());
    msg->addArgv(ui->passwdLineEdit->text ());
    emit sendMsg (msg);
}
