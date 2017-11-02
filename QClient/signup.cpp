#include "signup.h"
#include "ui_signup.h"
#include <QInputDialog>

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
    connect (ui->setToolButton, &QToolButton::clicked, this, &SignUp::clickSetBtn);
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
        tcpMsg->connectToHost (*serverAddress, 6666);
    }
}

void SignUp::trySignUp()
{
    Message *msg = new Message(Message::SignUp);
    msg->addArgv (ui->nameLineEdit->text ());
    QByteArray passwordSHA256;
    passwordSHA256.append (ui->passwordLineEdit->text ());
    QByteArray hash = QCryptographicHash::hash (passwordSHA256, QCryptographicHash::Sha256);
    msg->addArgv (hash.toHex ());
    msg->addArgv (ui->pwQuestionComboBox->currentText ());
    QByteArray pwAnswerSHA256;
    pwAnswerSHA256.append (ui->pwAnswerLineEdit->text ());
    QByteArray hashpwAnswer = QCryptographicHash::hash (pwAnswerSHA256, QCryptographicHash::Sha256);
    msg->addArgv (hashpwAnswer.toHex ());
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
            *id = static_cast<quint32>(msg->getArgv (0).toInt ());
            accept ();
        }
        else {
            ui->showLabel->setText (tr("用户名重复"));
            ui->nameLineEdit->clear ();
            ui->passwordLineEdit->clear ();
            ui->checkPasswordLineEdit->clear ();
            ui->pwAnswerLineEdit->clear ();
            ui->nameLineEdit->setFocus ();
        }
        break;
    default:
        break;
    }
    delete msg;
}

void SignUp::clickSetBtn()
{
    bool hadInput;
    QString str = QInputDialog::getText (this, tr("设置"), tr("请输入服务器地址"), QLineEdit::Normal, tr("255.255.255.255"), &hadInput);
    if (hadInput) {
        *serverAddress = QHostAddress(str);
    }
}

void SignUp::closeWindow()
{
    reject ();
}

