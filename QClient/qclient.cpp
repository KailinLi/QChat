#include "qclient.h"
#include "ui_qclient.h"
#include <QMessageBox>

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
    if (! server->listen (QHostAddress("127.0.0.1"))) {
        QMessageBox::critical(this, tr("Threaded Fortune Server"),
                              tr("Unable to start the server: %1.")
                              .arg(server->errorString()));
        close();
        return;
    }
//    connect (server, &ParallelServer::newConnection, this, &QServer::haveNewConnect);
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
    case Message::InitMsg: {
        for (int i = 0; i < msg->getArgv (0).toInt (); ++i) {
            userList.addUser (static_cast<quint32>(msg->getArgv (1 + 5 * i).toInt ()),
                              msg->getArgv (2 + 5 * i),
                              msg->getArgv (3 + 5 * i),
                              static_cast<quint16>(msg->getArgv (4 + 5 * i).toInt ()),
                              ((QString::compare (msg->getArgv (5 + 5 * i), "y")) ? false : true));
        }
        initUpdateUI ();
        Message *newMsg = new Message(Message::FinishInit);
        newMsg->addArgv (tr("127.0.0.1"));
        newMsg->addArgv (QString::number (server->serverPort ()));
        tcpToServer->send (newMsg);
        break;
    }
    case Message::UpdateMsg:
        if (msg->argc () != 1) {
            userList.newSignIn (static_cast<quint32>(msg->getArgv (0).toInt ()),
                                msg->getArgv (1),
                                msg->getArgv (2),
                                static_cast<quint16>(msg->getArgv (3).toInt ()));

        }
    default:
        break;
    }
    delete msg;
}

void QClient::initUpdateUI()
{
    QList<UserInfo> *p = userList.getList ();
    int onlineCount = 0;
    foreach (UserInfo user, *p) {
        if (user.getUserID () == userID) {
            userName = user.getName ();
            ui->userLabel->setText (tr("当前用户: %1").arg(userName));
        }
        QTableWidgetItem *name = new QTableWidgetItem(user.getName ());
        QTableWidgetItem *ifOnline;
        if (user.getIfOnline ()) {
            ++onlineCount;
            ifOnline = new QTableWidgetItem(tr("在线"));
        }
        else {
            ifOnline = new QTableWidgetItem(tr("离线"));
        }
        ui->userTableWidget->insertRow (0);
        ui->userTableWidget->setItem (0, 0, name);
        ui->userTableWidget->setItem (0, 1, ifOnline);
    }
    ui->onlineLabel->setText (tr("在线: %1").arg (onlineCount));
}

void QClient::signInUpdateUI(const QString &name)
{
    QList<QTableWidgetItem*> find = ui->userTableWidget->findItems (name, Qt::MatchExactly);
    if (find.isEmpty ()) {
        QTableWidgetItem *name = new QTableWidgetItem(name);
        QTableWidgetItem *ifOnline = new QTableWidgetItem(tr("在线"));
    }
}
