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
    connect (thread, &ConnectThread::newMsg, this, &QServer::haveNewMsg, Qt::QueuedConnection);
    connect (thread, &ConnectThread::finished, thread, &ConnectThread::deleteLater);
    connect (this, &QServer::msgToSend, thread, &ConnectThread::sendMsg, Qt::QueuedConnection);
    connect (thread, &ConnectThread::loseConnect, this, &QServer::loseConnect, Qt::QueuedConnection);
    threadPool.addThread (thread);
}

void QServer::haveNewMsg(ConnectThread *thread, Message *msg)
{
    switch (msg->getType ()) {
    case Message::SignIn:
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
    case Message::SignUp:
        if (userList.checkUserName (msg->getArgv (0))) {
            quint32 id = userList.newSignUp (msg->getArgv (0), msg->getArgv (1), msg->getArgv (2), msg->getArgv (3));
            Message *newMsg = new Message(Message::AnswerSignUp);
            newMsg->addArgv (QString::number (id));
            emit msgToSend (thread, msg);
        }
        else {
            Message *newMsg = new Message(Message::AnswerSignUp);
            newMsg->addArgv (tr("d"));
            emit msgToSend (thread, newMsg);
        }
        break;
    default:
        break;
    }
    delete msg;//important!!
}

void QServer::loseConnect(ConnectThread *thread)
{
    if (!thread->getUserID ()) {
        threadPool.removeThread (thread);
    }
//    else {

//    }
}
