#include "qserver.h"
#include "ui_qserver.h"
#include <QMessageBox>

QServer::QServer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QServer),
    server(new ParallelServer(this))
{
    ui->setupUi(this);
    userList.newSignUp (tr("likailin"), tr("1234"), tr("你计网实验的成绩是多少?"), tr("100"));
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
    server->close ();
    delete server;
    delete ui;
}

void QServer::msgSignIn(ConnectThread *thread, Message *msg)
{
    quint32 id = userList.ifPasswordRight (msg->getArgv (0), msg->getArgv (1));
    if (id != 0) {
        Message* newMsg = new Message(Message::AnswerSignIn);
        newMsg->addArgv (QString::number (id));
        emit msgToSend (thread, newMsg);
    }
    else {
        Message* newMsg = new Message(Message::AnswerSignIn);
        newMsg->addArgv (tr("n"));
        emit msgToSend (thread, newMsg);
    }
}

void QServer::msgSignUp(ConnectThread *thread, Message *msg)
{
    if (userList.checkUserName (msg->getArgv (0))) {
        quint32 id = userList.newSignUp (msg->getArgv (0), msg->getArgv (1), msg->getArgv (2), msg->getArgv (3));
        Message *newMsg = new Message(Message::AnswerSignUp);
        newMsg->addArgv (QString::number (id));
        emit msgToSend (thread, newMsg);
    }
    else {
        Message *newMsg = new Message(Message::AnswerSignUp);
        newMsg->addArgv (tr("d"));
        emit msgToSend (thread, newMsg);
    }
}

void QServer::msgForgetPassword(ConnectThread *thread, Message *msg)
{
    UserInfo* findP = userList.findPassword (msg->getArgv (0));
    if (!findP) {
        Message *newMsg = new Message(Message::AnswerForgetPassword);
        newMsg->addArgv (tr("n"));
        emit msgToSend (thread, newMsg);
    }
    else {
        Message *newMsg = new Message(Message::AnswerForgetPassword);
        newMsg->addArgv (findP->getPwQuestion ());
        newMsg->addArgv (findP->getPwAnswer ());
        newMsg->addArgv (findP->getPassword ());
        emit msgToSend (thread, newMsg);
    }
}

void QServer::msgLogIn(ConnectThread *thread, Message *msg)
{
    thread->setUserID (static_cast<quint32>(msg->getArgv (0).toInt ()));
    Message *newMsg = new Message(Message::InitMsg);
    userList.makeInitMsg (thread->getUserID (), newMsg);
    emit msgToSend (thread, newMsg);
}

void QServer::msgFinishInit(ConnectThread *thread, Message *msg)
{
    QString name = userList.newSignIn(thread->getUserID (), msg->getArgv (0), static_cast<quint16>(msg->getArgv (1).toInt ()));
    QList<ConnectThread *> *p = threadPool.getPool ();
    foreach (ConnectThread* t, *p) {
        if (t->getUserID ()) {
            Message *newMsg = new Message(Message::UpdateMsg);
            newMsg->addArgv (QString::number (thread->getUserID ()));
            newMsg->addArgv (name);
            newMsg->addArgv (msg->getArgv (0));
            newMsg->addArgv (msg->getArgv (1));
            emit msgToSend (t, newMsg);
        }
    }
}

void QServer::msgOfflineMsg(ConnectThread *thread, Message *msg)
{
    UserInfo *user = userList.getUser (static_cast<quint32>(msg->getArgv (0).toInt ()));
    user->msgQueue.enqueue (QPair<quint32, QString>(thread->getUserID (), msg->getArgv (1)));
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
        msgSignIn (thread, msg);
        break;
    case Message::SignUp:
        msgSignUp (thread, msg);
        break;
    case Message::ForgetPassword:
        msgForgetPassword (thread, msg);
        break;
    case Message::LogIn:
        msgLogIn (thread, msg);
        break;
    case Message::FinishInit:
        msgFinishInit (thread, msg);
        break;
    case Message::OfflineMsg:

    default:
        break;
    }
    delete msg;//important!!
}

void QServer::loseConnect(ConnectThread *thread)
{
//    if (!thread->getUserID ()) {
//        threadPool.removeThread (thread);
//    }
    if (thread->getUserID ()) {
        userList.userSignOut (thread->getUserID ());
        QList<ConnectThread *> *p = threadPool.getPool ();
        foreach (ConnectThread* t, *p) {
            if (t->getUserID ()) {
                Message *newMsg = new Message(Message::UpdateMsg);
                newMsg->addArgv (QString::number (thread->getUserID ()));
                emit msgToSend (t, newMsg);
            }
        }
    }
    threadPool.removeThread (thread);
}
