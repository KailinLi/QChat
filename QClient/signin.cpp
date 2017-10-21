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
    connect (tcpMsg, &TcpSocketMsg::readyRead, this, &SignIn::haveNewMsgFromServer);
    connect (this, &SignIn::sendMsg, tcpMsg, &TcpSocketMsg::send);
    connect (ui->exitBtn, &QPushButton::clicked, this, &SignIn::closeWindow);
    connect (ui->signUpBtn, &QPushButton::clicked, this, &SignIn::callSignUp);
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
    tcpMsg->abort ();
    tcpMsg->connectToHost (QHostAddress(tr("127.0.0.1")), 6666);
}

void SignIn::trySignIn()
{
    Message *msg = new Message(Message::SignIn);
    msg->addArgv(ui->nameLineEdit->text ());
    msg->addArgv(ui->passwdLineEdit->text ());
    emit sendMsg (msg);
}

void SignIn::haveNewMsgFromServer()
{
    Message *msg = tcpMsg->read ();
    if (msg == nullptr) return;
    switch (msg->getType ()) {
    case Message::AnswerSignIn:
        qDebug() << msg->getArgv (0);
        if (QString::compare (msg->getArgv (0), tr("y"))) {
            ui->showLabel->setText (tr("用户名或密码错误"));
            ui->nameLineEdit->clear ();
            ui->passwdLineEdit->clear ();
            ui->nameLineEdit->setFocus ();
        }
        else {
            tcpMsg->safeDelete ();
            done (SignIn::SignInSuccess);
        }
        break;
    default:
        break;
    }
    delete msg;
}

void SignIn::callSignUp()
{
    tcpMsg->safeDelete ();
    done (SignIn::SignUp);
}

void SignIn::closeWindow()
{
    done (SignIn::Exit);
}
