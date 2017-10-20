#include "qserver.h"
#include "ui_qserver.h"
#include <QMessageBox>

QServer::QServer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QServer),
    server(new ParallelServer(this))
{
    ui->setupUi(this);
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
    threadPool.addThread (thread);
    connect (thread, &ConnectThread::newMsg, this, &QServer::haveNewMsg);
    connect (thread, &ConnectThread::finished, thread, &ConnectThread::deleteLater);
}

void QServer::haveNewMsg(ConnectThread *thread, Message *msg)
{
    switch (msg->getType ()) {
    case Message::SignIn:
        qDebug() << msg->getArgv (0);
        qDebug() << msg->getArgv (1);
        break;
    default:
        break;
    }
}
