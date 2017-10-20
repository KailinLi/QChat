#include "qserver.h"
#include "ui_qserver.h"
#include <QMessageBox>

QServer::QServer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QServer),
    server(new ParallelServer(this))
{
    ui->setupUi(this);
    userList.newSignUp (tr("likailin"), tr("1234"), tr(""), tr(""));
    if (! server->listen (QHostAddress("127.0.0.1"), 6666)) {
        QMessageBox::critical(this, tr("Threaded Fortune Server"),
                              tr("Unable to start the server: %1.")
                              .arg(server->errorString()));
        close();
        return;
    }
    connect (server, &ParallelServer::newConnection, this, &QServer::haveNewConnect);

}

QServer::~QServer()
{
    delete ui;
}

void QServer::haveNewConnect(qintptr socketDescriptor)
{
    ConnectThread* thread = new ConnectThread(socketDescriptor, this);
    connect (thread, &ConnectThread::newMsg, this, &QServer::haveNewMsg);
    connect (thread, &ConnectThread::finished, thread, &ConnectThread::deleteLater);
    connect (this, &QServer::msgToSend, thread, &ConnectThread::sendMsg);
    connect (thread, &ConnectThread::loseConnect, this, &QServer::loseConnect);
    threadPool.addThread (thread);
}

void QServer::haveNewMsg(ConnectThread *thread, Message *msg)
{
    switch (msg->getType ()) {
    case Message::SignIn:
        qDebug() << msg->getArgv (0);
        qDebug() << msg->getArgv (1);
        if (userList.ifPasswordRight (msg->getArgv (0), msg->getArgv (1))) {
            Message* newMsg = new Message(Message::AnswerSignIn);
            newMsg->addArgv (tr("y"));
            emit msgToSend (thread, newMsg);
        }
        else {
            Message* newMsg = new Message(Message::AnswerSignIn);
            newMsg->addArgv (tr("n"));
            emit msgToSend (thread, newMsg);
        }
        break;
    default:
        break;
    }
}

void QServer::loseConnect(ConnectThread *thread)
{
    if (!thread->getUserID ()) {
        threadPool.removeThread (thread);
    }
//    else {

//    }
}
