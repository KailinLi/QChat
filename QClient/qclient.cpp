#include "qclient.h"
#include "ui_qclient.h"
#include <QMessageBox>

QClient::QClient(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QClient),
    server(new ParallelServer(this)),
    tcpToServer(new TcpSocketMsg(this)),
    currentID(1)
{
    ui->setupUi(this);
    connect (tcpToServer, &TcpSocketMsg::connected, this, &QClient::logIn);
    connect (tcpToServer, &TcpSocketMsg::readyRead, this, &QClient::haveNewMsgFromServer);
    connect (this, &QClient::sendMsg, tcpToServer, &TcpSocketMsg::send);
    connect (ui->exitBtn, &QPushButton::clicked, this, &QClient::quit);
    ui->userTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect (ui->userTableWidget, &QTableWidget::cellClicked, this, &QClient::makeActive);
    connect (ui->userTableWidget, &QTableWidget::cellDoubleClicked, this, &QClient::makeActive);
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
            signInUpdateUI (msg->getArgv (1));
        }
        else {
            QString name = userList.userSignOut (static_cast<quint32>(msg->getArgv (0).toInt ()));
            signOutUpdateUI (name);
        }
        break;
    default:
        break;
    }
    delete msg;
}

void QClient::initUpdateUI()
{
    QList<UserInfo> *p = userList.getList ();
    foreach (UserInfo user, *p) {
        if (user.getUserID () == userID) {
            userName = user.getName ();
            ui->userLabel->setText (tr("当前用户: %1").arg(userName));
        }
        QTableWidgetItem *name = new QTableWidgetItem(user.getName ());
        QTableWidgetItem *ifOnline;
        if (user.getIfOnline ()) {
            ifOnline = new QTableWidgetItem(tr("在线"));
        }
        else {
            ifOnline = new QTableWidgetItem(tr("离线"));
        }
        ui->userTableWidget->insertRow (0);
        ui->userTableWidget->setItem (0, 0, name);
        ui->userTableWidget->setItem (0, 1, ifOnline);
    }
    ui->onlineLabel->setText (tr("在线: %1").arg (userList.getOnlineCount ()));
}

void QClient::signInUpdateUI(const QString &name)
{
    QList<QTableWidgetItem*> find = ui->userTableWidget->findItems (name, Qt::MatchExactly);
    if (find.isEmpty ()) {
        QTableWidgetItem *userName = new QTableWidgetItem(name);
        QTableWidgetItem *ifOnline = new QTableWidgetItem(tr("在线"));
        ui->userTableWidget->insertRow (0);
        ui->userTableWidget->setItem (0, 0, userName);
        ui->userTableWidget->setItem (0, 1, ifOnline);
    }
    else {
        int row = find.at (0)->row ();
        QTableWidgetItem *ifOnline = new QTableWidgetItem(tr("在线"));
        ui->userTableWidget->setItem (row, 1, ifOnline);
    }
    ui->onlineLabel->setText (tr("在线: %1").arg (userList.getOnlineCount ()));
}

void QClient::signOutUpdateUI(const QString &name)
{
    QList<QTableWidgetItem*> find = ui->userTableWidget->findItems (name, Qt::MatchExactly);
    int row = find.at (0)->row ();
    QTableWidgetItem *ifOnline = new QTableWidgetItem(tr("离线"));
    ui->userTableWidget->setItem (row, 1, ifOnline);
    ui->onlineLabel->setText (tr("在线: %1").arg (userList.getOnlineCount ()));
}

void QClient::quit()
{
    close ();
}

void QClient::makeActive(int row)
{
    QTableWidgetItem *item = ui->userTableWidget->item (row, 0);
    QString name = item->text ();
    QPair<quint32, bool> info;
    userList.getActiveInfo (name, info);
    if (info.first == userID || info.first == currentID) return;
//    qDebug() << info.second;
//    if (info.second) {
//        if (!threadPool.getThread (info.first)) {
//            qDebug() << "make new thread";
//        }
//    }
    userList.saveMsg (currentID, ui->msgBrowser->toHtml ());
    currentID = info.first;
    ui->msgBrowser->setHtml (userList.getMsg (currentID));
    UserInfo *user = userList.getUser (info.first);
    while (!user->msgQueue.isEmpty ()) {
//        format.setAlignment(Qt::AlignRight);
        ui->msgBrowser->setTextColor (Qt::blue);
//        setCurrentCharFormat
        ui->msgBrowser->setCurrentFont (QFont("Times new Roman", 12));
        ui->msgBrowser->append (user->getName ());
        ui->msgBrowser->append (user->msgQueue.dequeue ());
    }
}

void QClient::changeTableWidget(quint32 id)
{

}
