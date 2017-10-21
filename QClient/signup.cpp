#include "signup.h"
#include "ui_signup.h"

SignUp::SignUp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SignUp),
    tcpMsg(new TcpSocketMsg(this))
{
    ui->setupUi(this);
    connect (ui->signUpBtn, &QPushButton::clicked, this, &SignUp::tryConnect);
    connect (tcpMsg, &TcpSocketMsg::connected, this, &SignUp::trySignUp);
    connect (tcpMsg, &TcpSocketMsg::readyRead, this, &SignUp::haveNewMsgFromServer);
    connect (this, &SignUp::sendMsg, tcpMsg, &TcpSocketMsg::send);
    connect (ui->exitBtn, &QPushButton::clicked, this, &SignUp::closeWindow);
    ui->nameLineEdit->setFocus ();
    ui->signUpBtn->setAutoDefault (true);
    ui->signUpBtn->setDefault (true);
}

SignUp::~SignUp()
{
    delete ui;
}

void SignUp::tryConnect()
{
    if (ui->nameLineEdit->text ().isEmpty () ||
            ui->passwordLineEdit->text ().isEmpty () ||
            ui->checkPasswordLineEdit->text ().isEmpty () ||
            ui->pwQuestionComboBox->currentText ().isEmpty () ||
            ui->pwAnswerLineEdit->text ().isEmpty ()) {
        ui->showLabel->setText (tr("输入信息不完整"));
    }
    else if (QString::compare (ui->passwordLineEdit->text (), ui->checkPasswordLineEdit->text ())) {
        ui->showLabel->setText (tr("两次输入密码不一致"));
        ui->passwordLineEdit->clear ();
        ui->checkPasswordLineEdit->clear ();
        ui->passwordLineEdit->setFocus ();
    }
    else {
        tcpMsg->setBlockSize (0);
        tcpMsg->abort ();
        tcpMsg->connectToHost (QHostAddress(tr("127.0.0.1")), 6666);
    }
}

void SignUp::trySignUp()
{
    Message *msg = new Message(Message::SignUp);
    msg->addArgv (ui->nameLineEdit->text ());
    msg->addArgv (ui->passwordLineEdit->text ());
    msg->addArgv (ui->pwQuestionComboBox->currentText ());
    msg->addArgv (ui->pwAnswerLineEdit->text ());
    emit sendMsg (msg);
}

void SignUp::haveNewMsgFromServer()
{
    Message *msg = tcpMsg->read ();
    if (msg == nullptr) return;
    switch (msg->getType ()) {
    case Message::AnswerSignUp:
        if (QString::compare (msg->getArgv (0), tr("d"))) {
            tcpMsg->safeDelete ();
            accept ();
        }
        else {
            ui->showLabel->setText (tr("用户名重复"));
            ui->nameLineEdit->clear ();
            ui->passwordLineEdit->clear ();
            ui->checkPasswordLineEdit->clear ();
            ui->pwAnswerLineEdit->clear ();
            ui->pwQuestionComboBox->clear ();
            ui->nameLineEdit->setFocus ();
        }
        break;
    default:
        break;
    }
    delete msg;
}

void SignUp::closeWindow()
{
    reject ();
}

