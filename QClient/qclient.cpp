#include "qclient.h"
#include "ui_qclient.h"

QClient::QClient(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QClient),
    server(new ParallelServer(this)),
    tcpToServer(new TcpSocketMsg(this))
{
    ui->setupUi(this);
    connect (tcpToServer, &TcpSocketMsg::connected, this, &QClient::logIn);
    connect (tcpToServer, &TcpSocketMsg::readyRead, this, &QClient::haveNewMsgFromServer);
    connect (this, &QClient::sendMsg, tcpToServer, &TcpSocketMsg::send);
}

QClient::~QClient()
{
    server->close ();
    delete server;
    tcpToServer->safeDelete ();
    delete ui;
}

void QClient::start()
{
    tcpToServer->abort ();
    qDebug() << userID;
    tcpToServer->setBlockSize (0);
    tcpToServer->connectToHost (QHostAddress(tr("127.0.0.1")), 6666);
}

void QClient::logIn()
{
    Message *msg = new Message(Message::LogIn);
    msg->addArgv(QString::number (userID));
    emit sendMsg (msg);
}

void QClient::haveNewMsgFromServer()
{
    Message *msg = tcpToServer->read ();
    if (msg == nullptr) return;
    switch (msg->getType ()) {
    case Message::InitMsg:

        break;
    default:
        break;
    }
    delete msg;
}
