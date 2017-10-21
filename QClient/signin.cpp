#include "signin.h"
#include "ui_signin.h"
#include <QInputDialog>

SignIn::SignIn(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SignIn),
    tcpMsg(new TcpSocketMsg(this)),
    needPassword(false)
{
    ui->setupUi(this);
    ui->passwdLineEdit->setEchoMode (QLineEdit::Password);
    connect (ui->signinBtn, &QPushButton::clicked, this, &SignIn::tryConnect);
    connect (tcpMsg, &TcpSocketMsg::connected, this, &SignIn::trySignIn);
    connect (tcpMsg, &TcpSocketMsg::connected, this, &SignIn::tryFindPassword);
    connect (tcpMsg, &TcpSocketMsg::readyRead, this, &SignIn::haveNewMsgFromServer);
    connect (this, &SignIn::sendMsg, tcpMsg, &TcpSocketMsg::send);
    connect (ui->exitBtn, &QPushButton::clicked, this, &SignIn::closeWindow);
    connect (ui->signUpBtn, &QPushButton::clicked, this, &SignIn::callSignUp);
    connect (ui->forgetPasswordBtn, &QPushButton::clicked, this, &SignIn::forgetPassword);
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
    if (needPassword) return;
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
    case Message::AnswerForgetPassword:
        if (!QString::compare (msg->getArgv (0), "n")) {
            ui->showLabel->setText (tr("没有此用户"));
            ui->nameLineEdit->clear ();
            ui->passwdLineEdit->clear ();
            ui->nameLineEdit->setFocus ();
        }
        else if (showFindPasswordDialog (msg->getArgv (0), msg->getArgv (1))) {
            ui->showLabel->setText (tr("密码:%1, 请牢记哦").arg (msg->getArgv (2)));
//            tcpMsg->safeDelete ();
        }
        else {
            ui->showLabel->setText (tr("密保问题错误"));
            ui->nameLineEdit->clear ();
            ui->passwdLineEdit->clear ();
            ui->nameLineEdit->setFocus ();
        }
        break;
    default:
        break;
    }
    delete msg;
}

void SignIn::tryFindPassword()
{
    if (!needPassword) return;
    Message *msg = new Message(Message::ForgetPassword);
    msg->addArgv(ui->nameLineEdit->text ());
    emit sendMsg (msg);
    needPassword = false;
}

bool SignIn::showFindPasswordDialog(const QString &pwQuestion, const QString &pwAnswer)
{
    bool hadInput;
    QString str = QInputDialog::getText (this, tr("找回密码"), pwQuestion, QLineEdit::Normal, tr("请输入密保答案"), &hadInput);
    if (hadInput) {
        if (QString::compare (str, pwAnswer)) return false;
        else return true;
    }
    else return false;
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

void SignIn::forgetPassword()
{
    if (ui->nameLineEdit->text ().isEmpty ()) {
        ui->showLabel->setText (tr("请输入用户名"));
        return;
    }
    needPassword = true;
    tryConnect ();
}
