#include "signin.h"
#include "ui_signin.h"
#include <QInputDialog>
#include <QCryptographicHash>

SignIn::SignIn(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SignIn),
    tcpMsg(new TcpSocketMsg(this)),
    needPassword(false),
    resetPassword(false)
{
    ui->setupUi(this);
    ui->passwdLineEdit->setEchoMode (QLineEdit::Password);
    connect (ui->signinBtn, &QPushButton::clicked, this, &SignIn::tryConnect);
    connect (tcpMsg, &TcpSocketMsg::connected, this, &SignIn::trySignIn);
    connect (tcpMsg, &TcpSocketMsg::connected, this, &SignIn::tryFindPassword);
    connect (tcpMsg, &TcpSocketMsg::connected, this , &SignIn::tryResetPassword);
    connect (tcpMsg, &TcpSocketMsg::readyRead, this, &SignIn::haveNewMsgFromServer);
    connect (ui->setToolButton, &QToolButton::clicked, this, &SignIn::clickSetBtn);
    connect (this, &SignIn::sendMsg, tcpMsg, &TcpSocketMsg::send);
    connect (ui->exitBtn, &QPushButton::clicked, this, &SignIn::closeWindow);
    connect (ui->signUpBtn, &QPushButton::clicked, this, &SignIn::callSignUp);
    connect (ui->forgetPasswordBtn, &QPushButton::clicked, this, &SignIn::forgetPassword);
    ui->nameLineEdit->setFocus ();
    ui->signinBtn->setAutoDefault (true);
    ui->signinBtn->setDefault (true);
}

SignIn::~SignIn()
{
    delete ui;
}

void SignIn::tryConnect()
{
    if (resetPassword) {
        if (QString::compare (ui->nameLineEdit->text (), ui->passwdLineEdit->text ())) {
            ui->showLabel->setText (tr("两次密码不一致"));
            ui->nameLineEdit->clear ();
            ui->passwdLineEdit->clear ();
            ui->nameLineEdit->setFocus ();
            return;
        }
    }
    tcpMsg->setBlockSize (0);
    tcpMsg->abort ();
    tcpMsg->connectToHost (*serverAddress, 16666);
}

void SignIn::trySignIn()
{
    if (needPassword) return;
    if (resetPassword) return;
    Message *msg = new Message(Message::SignIn);
    msg->addArgv(ui->nameLineEdit->text ());
    QByteArray passwordSHA256;
    passwordSHA256.append (ui->passwdLineEdit->text ());
    QByteArray hash = QCryptographicHash::hash (passwordSHA256, QCryptographicHash::Sha256);
    msg->addArgv (hash.toHex ());
    emit sendMsg (msg);
}

void SignIn::haveNewMsgFromServer()
{
    Message *msg = tcpMsg->read ();
    if (msg == nullptr) return;
    switch (msg->getType ()) {
    case Message::AnswerSignIn:
        if (QString::compare (msg->getArgv (0), tr("n"))) {
            tcpMsg->safeDelete ();
            *id = static_cast<quint32>(msg->getArgv (0).toInt ());
            done (SignIn::SignInSuccess);
        }
        else {
            ui->showLabel->setText (tr("用户名或密码错误"));
            ui->nameLineEdit->clear ();
            ui->passwdLineEdit->clear ();
            ui->nameLineEdit->setFocus ();
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
            ui->showLabel->setText (tr("请重设密码"));
            ui->nameLabel->setText (tr("密码:"));
            ui->passwdLabel->setText (tr("再次输入密码:"));
            ui->signinBtn->setText (tr("确定"));
            ui->nameLineEdit->setEchoMode (QLineEdit::Password);
            resetPassword = true;
            inputNameTmp = ui->nameLineEdit->text ();
            ui->nameLineEdit->clear ();
            ui->nameLabel->setFocus ();
        }
        else {
            ui->showLabel->setText (tr("密保问题错误"));
            ui->nameLineEdit->clear ();
            ui->passwdLineEdit->clear ();
            ui->nameLineEdit->setFocus ();
        }
        break;
    case Message::AnswerResetPassword:
        ui->showLabel->setText (tr("密码重置成功"));
        ui->nameLabel->setText (tr("用户名:"));
        ui->passwdLabel->setText (tr("密码:"));
        ui->signinBtn->setText (tr("登录"));
        ui->nameLineEdit->setEchoMode (QLineEdit::Normal);
        ui->nameLineEdit->clear ();
        ui->passwdLineEdit->clear ();
        ui->nameLineEdit->setFocus ();
        resetPassword = false;
        break;
    default:
        break;
    }
    delete msg;
}

void SignIn::tryFindPassword()
{
    if (!needPassword) return;
    if (resetPassword) return;
    Message *msg = new Message(Message::ForgetPassword);
    msg->addArgv(ui->nameLineEdit->text ());
    emit sendMsg (msg);
    needPassword = false;
}

void SignIn::tryResetPassword()
{
    if (!resetPassword) return;
    if (needPassword) return;
    Message *msg = new Message(Message::ResetPassword);
    msg->addArgv (inputNameTmp);
    QByteArray passwordSHA256;
    passwordSHA256.append (ui->passwdLineEdit->text ());
    QByteArray hash = QCryptographicHash::hash (passwordSHA256, QCryptographicHash::Sha256);
    msg->addArgv (hash.toHex ());
    emit sendMsg (msg);
}

bool SignIn::showFindPasswordDialog(const QString &pwQuestion, const QString &pwAnswer)
{
    bool hadInput;
    QString str = QInputDialog::getText (this, tr("找回密码"), pwQuestion, QLineEdit::Normal, tr("请输入密保答案"), &hadInput);
    if (hadInput) {
        QByteArray passwordSHA256;
        passwordSHA256.append (str);
        QByteArray hash = QCryptographicHash::hash (passwordSHA256, QCryptographicHash::Sha256);
        if (QString::compare (hash.toHex (), pwAnswer)) return false;
        else return true;
    }
    else return false;
}

void SignIn::clickSetBtn()
{
    bool hadInput;
    QString str = QInputDialog::getText (this, tr("设置"), tr("请输入服务器地址"), QLineEdit::Normal, tr("255.255.255.255"), &hadInput);
    if (hadInput) {
        *serverAddress = QHostAddress(str);
    }
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
