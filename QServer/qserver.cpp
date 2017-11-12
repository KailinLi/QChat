#include "qserver.h"
#include "ui_qserver.h"
#include <QMessageBox>
#include "handledir.cpp"

QServer::QServer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QServer),
    server(new ParallelServer(this))
{
    ui->setupUi(this);
    loadUserInfo ();
    localAddress = QHostAddress("127.0.0.1");
    if (! server->listen (localAddress, 16666)) {
        QMessageBox::critical(this, tr("Fail Listening"),
                              tr("Unable to start the server: %1.")
                              .arg(server->errorString()));
        close();
        return;
    }
    connect (server, &ParallelServer::newConnection, this, &QServer::haveNewConnect);
    connect (ui->setToolButton, &QToolButton::clicked, this, &QServer::clickSetBtn);
    ui->showLabel->setText (tr("服务器地址 %1 监听端口%2").arg (localAddress.toString ()).arg (QString::number (16666)));
    initUI ();
}

QServer::~QServer()
{
    saveUserInfo ();
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
        emit msgToSend (thread, newMsg);
    }
}

void QServer::msgResetPassword(ConnectThread *thread, Message *msg)
{
    UserInfo* findP = userList.findPassword (msg->getArgv (0));
    findP->setPassword (msg->getArgv(1));
    Message *newMsg = new Message(Message::AnswerResetPassword);
    emit msgToSend (thread, newMsg);
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

    QList<QTableWidgetItem*> find = ui->userTableWidget->findItems (name, Qt::MatchExactly);
    if (find.size ()) {
        int row = find.at (0)->row ();
        QTableWidgetItem *addressItem = new QTableWidgetItem(msg->getArgv (0));
        QTableWidgetItem *portItem = new QTableWidgetItem(msg->getArgv (1));
        QTableWidgetItem *ifOnlineItem = new QTableWidgetItem(tr("在线"));
        ui->userTableWidget->setItem (row, 1, addressItem);
        ui->userTableWidget->setItem (row, 2, portItem);
        ui->userTableWidget->setItem (row, 3, ifOnlineItem);
    }
    else {
        ui->userTableWidget->insertRow (0);
        QTableWidgetItem *nameItem = new QTableWidgetItem(name);
        QTableWidgetItem *addressItem = new QTableWidgetItem(msg->getArgv (0));
        QTableWidgetItem *portItem = new QTableWidgetItem(msg->getArgv (1));
        QTableWidgetItem *ifOnlineItem = new QTableWidgetItem(tr("在线"));
        ui->userTableWidget->setItem (0, 0, nameItem);
        ui->userTableWidget->setItem (0, 1, addressItem);
        ui->userTableWidget->setItem (0, 2, portItem);
        ui->userTableWidget->setItem (0, 3, ifOnlineItem);
    }

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

void QServer::loadUserInfo()
{
    QFile file (getPath ().append ("/userinfo"));

    if (!file.open (QIODevice::ReadOnly)) {
        qDebug() << "can not open file";
        return;
    }
    QDataStream in (&file);
    in.setVersion (QDataStream::Qt_5_6);

    userList.readUserInfo (in);

    file.close ();
}

void QServer::saveUserInfo()
{
    QFile file (getPath ().append ("/userinfo"));
    if (!file.open (QIODevice::WriteOnly | QIODevice::Truncate)) {
        qDebug() << "can not open file";
        return;
    }
    QDataStream out (&file);
    out.setVersion (QDataStream::Qt_5_6);

    userList.saveUserInfo (out);

    file.close ();
}

void QServer::initUI()
{
    for (int i = 1; i <= userList.size (); ++i) {
        QTableWidgetItem *nameItem = new QTableWidgetItem(userList.getUserName (i));
        QTableWidgetItem *addressItem = new QTableWidgetItem();
        QTableWidgetItem *portItem = new QTableWidgetItem();
        QTableWidgetItem *ifOnlineItem = new QTableWidgetItem(tr("离线"));
        ui->userTableWidget->insertRow (0);
        ui->userTableWidget->setItem (0, 0, nameItem);
        ui->userTableWidget->setItem (0, 1, addressItem);
        ui->userTableWidget->setItem (0, 2, portItem);
        ui->userTableWidget->setItem (0, 3, ifOnlineItem);
    }
}

QHostAddress QServer::getIP()
{
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    for (auto address = list.begin (); address != list.end (); ++address) {
        if(address->protocol() == QAbstractSocket::IPv4Protocol && *address != QHostAddress(QHostAddress::LocalHost)) {
            localAddress = *address;
            return *address;
        }
    }
//    foreach (QHostAddress address, list) {
//        if(address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost)) {
//            localAddress = address;
//            return address;
//        }
//    }
    return QHostAddress();
}

void QServer::clickSetBtn(bool checked)
{
    if (checked) {
        server->close ();
        while (server->isListening ()) {}
        if (! server->listen (getIP (), 16666)) {
            QMessageBox::critical(this, tr("Threaded Fortune Server"),
                                  tr("Unable to start the server: %1.")
                                  .arg(server->errorString()));
            close();
            return;
        }
        ui->showLabel->setText (tr("服务器地址 %1 监听端口%2").arg (localAddress.toString ()).arg (QString::number (16666)));
    }

    else {
        server->close ();
        localAddress = QHostAddress("127.0.0.1");
        while (server->isListening ()) {}
        if (! server->listen (localAddress, 16666)) {
            QMessageBox::critical(this, tr("Threaded Fortune Server"),
                                  tr("Unable to start the server: %1.")
                                  .arg(server->errorString()));
            close();
            return;
        }
        ui->showLabel->setText (tr("服务器地址 %1 监听端口%2").arg (localAddress.toString ()).arg (QString::number (16666)));
    }
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
    case Message::ResetPassword:
        msgResetPassword (thread, msg);
        break;
    case Message::LogIn:
        msgLogIn (thread, msg);
        break;
    case Message::FinishInit:
        msgFinishInit (thread, msg);
        break;
    case Message::OfflineMsg:
        msgOfflineMsg (thread, msg);
        break;
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

        QList<QTableWidgetItem*> find = ui->userTableWidget->findItems (userList.getUserName (thread->getUserID ()), Qt::MatchExactly);
        int row = find.at (0)->row ();
        QTableWidgetItem *addressItem = new QTableWidgetItem();
        QTableWidgetItem *portItem = new QTableWidgetItem();
        QTableWidgetItem *ifOnlineItem = new QTableWidgetItem(tr("离线"));
        ui->userTableWidget->setItem (row, 1, addressItem);
        ui->userTableWidget->setItem (row, 2, portItem);
        ui->userTableWidget->setItem (row, 3, ifOnlineItem);

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
