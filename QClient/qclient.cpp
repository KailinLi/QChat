#include "qclient.h"
#include "ui_qclient.h"
#include <QMessageBox>
#include <QKeyEvent>
#include <QColorDialog>
#include <QSet>
#include <QScrollBar>
#include <QTextBlock>

QClient::QClient(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QClient),
    server(new ParallelServer(this)),
    tcpToServer(new TcpSocketMsg(this)),
    currentID(0)
{
    ui->setupUi(this);
    connect (tcpToServer, &TcpSocketMsg::connected, this, &QClient::logIn);
    connect (tcpToServer, &TcpSocketMsg::readyRead, this, &QClient::haveNewMsgFromServer);
    connect (this, &QClient::sendMsg, tcpToServer, &TcpSocketMsg::send);
    connect (ui->exitBtn, &QPushButton::clicked, this, &QClient::quit);
    ui->userTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect (ui->userTableWidget, &QTableWidget::cellClicked, this, &QClient::makeActive);
    connect (ui->userTableWidget, &QTableWidget::cellDoubleClicked, this, &QClient::makeActive);
    connect (ui->sendBtn, &QPushButton::clicked, this, &QClient::clickSend);
    ui->msgTextEdit->installEventFilter (this);
    if (! server->listen (QHostAddress("127.0.0.1"))) {
        QMessageBox::critical(this, tr("Threaded Fortune Server"),
                              tr("Unable to start the server: %1.")
                              .arg(server->errorString()));
        close();
        return;
    }
    ui->msgTextEdit->setFocus ();
    ui->sendBtn->setAutoDefault (true);
    ui->sendBtn->setDefault (true);
    ui->userTableWidget->verticalHeader()->setVisible(false);
    connect (server, &ParallelServer::newConnection, this, &QClient::haveNewConnect);
    connect (ui->fontComboBox, &QFontComboBox::currentFontChanged, this, &QClient::currentFontChanged);
    connect (ui->sizeComboBox, &QComboBox::currentTextChanged, this, &QClient::currentFontSizeChanged);
    connect (ui->boldToolBtn, &QToolButton::clicked, this, &QClient::clickBoldBtn);
    connect (ui->italicToolBtn, &QToolButton::clicked, this, &QClient::clickItalicBtn);
    connect (ui->underlineToolBtn, &QToolButton::clicked, this, &QClient::clickUnderlineBtn);
    connect (ui->colorToolBtn, &QToolButton::clicked, this, &QClient::clickColorBtn);
//    ui->msgBrowser->setLineWrapMode(QTextEdit::NoWrap);
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
        int i = 0;
        for (; i < msg->getArgv (0).toInt (); ++i) {
            userList.addUser (static_cast<quint32>(msg->getArgv (1 + 5 * i).toInt ()),
                              msg->getArgv (2 + 5 * i),
                              msg->getArgv (3 + 5 * i),
                              static_cast<quint16>(msg->getArgv (4 + 5 * i).toInt ()),
                              ((QString::compare (msg->getArgv (5 + 5 * i), "y")) ? false : true));
        }
        i = 1 + 5 * i;
        int size = msg->getArgv (i++).toInt ();
        QSet<QString> dotList;
        for (int j = 0; j < size; ++j) {
            quint32 id = static_cast<quint32>(msg->getArgv (i + j * 2).toInt ());
            UserInfo *user = userList.getUser (id);
            user->msgQueue.enqueue (msg->getArgv (i + 1 + j * 2));
            dotList.insert (user->getName ());
        }
        initUpdateUI ();
        for (auto item = dotList.begin (); item != dotList.end (); ++item) {
            setRedDot (*item);
        }
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
    threadPool.closeAll ();
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
    changeTableWidget (info.first);
}

void QClient::changeTableWidget(quint32 id)
{
    userList.saveMsg (currentID, ui->msgBrowser->toHtml ());
    currentID = id;
    ui->msgBrowser->clear ();
    ui->msgBrowser->setHtml (userList.getMsg (currentID));
    UserInfo *user = userList.getUser (id);
    while (!user->msgQueue.isEmpty ()) {
        readMsgUpdateUI (user->getName (), user->msgQueue.dequeue ());
    }
    QList<QTableWidgetItem*> find = ui->userTableWidget->findItems (user->getName (), Qt::MatchExactly);
    int row = find.at (0)->row ();
    QTableWidgetItem *item = new QTableWidgetItem(user->getName ());
    item->setData (Qt::ForegroundRole, QColor(Qt::black));
    ui->userTableWidget->setItem (row, 0, item);
}

void QClient::sendMsgToUI()
{
    ui->msgBrowser->moveCursor (QTextCursor::NextBlock);
    ui->msgBrowser->setTextColor (Qt::red);
    ui->msgBrowser->setCurrentFont (QFont("Times new Roman", 12));
    ui->msgBrowser->append (userName + tr(" <"));
    QTextCursor cursor = ui->msgBrowser->textCursor();
    QTextBlockFormat textBlockFormat = cursor.blockFormat();
    textBlockFormat.setAlignment(Qt::AlignRight);
    cursor.mergeBlockFormat(textBlockFormat);
    ui->msgBrowser->setTextCursor(cursor);
    ui->msgBrowser->append (ui->msgTextEdit->toHtml ());
    cursor = ui->msgBrowser->textCursor();
    textBlockFormat = cursor.blockFormat();
    textBlockFormat.setAlignment(Qt::AlignRight);
    cursor.mergeBlockFormat(textBlockFormat);
    ui->msgBrowser->setTextCursor(cursor);
    ui->msgBrowser->verticalScrollBar ()->setValue (ui->msgBrowser->verticalScrollBar ()->maximum ());
    ui->msgTextEdit->clear ();
    ui->msgTextEdit->setFocus ();
}
void QClient::readMsgUpdateUI(const QString &name, const QString &msg)
{
    ui->msgBrowser->moveCursor (QTextCursor::NextBlock);
    ui->msgBrowser->setTextColor (Qt::blue);
    ui->msgBrowser->setCurrentFont (QFont("Times new Roman", 12));
    ui->msgBrowser->append (tr("> ") + name);
    QTextCursor cursor = ui->msgBrowser->textCursor();
    QTextBlockFormat textBlockFormat = cursor.blockFormat();
    textBlockFormat.setAlignment(Qt::AlignLeft);
    cursor.mergeBlockFormat(textBlockFormat);
    ui->msgBrowser->setTextCursor(cursor);
    ui->msgBrowser->append (msg);
    cursor = ui->msgBrowser->textCursor();
    textBlockFormat = cursor.blockFormat();
    textBlockFormat.setAlignment(Qt::AlignLeft);
    cursor.mergeBlockFormat(textBlockFormat);
    ui->msgBrowser->setTextCursor(cursor);
    ui->msgBrowser->verticalScrollBar ()->setValue (ui->msgBrowser->verticalScrollBar ()->maximum ());
}

void QClient::clickSend()
{
    if (ui->msgTextEdit->toPlainText ().isEmpty ()) {
        QMessageBox::warning (0, tr("警告"), tr("发送内容为空"), QMessageBox::Ok);
        return;
    }
    UserInfo *user = userList.getUser (currentID);
    if (user->getIfOnline ()) {
        ConnectThread* thread = threadPool.getThread (currentID);
        if (thread == nullptr) {
            ConnectThread* thread = new ConnectThread(user->getAddress (), user->getPort (), user->getUserID (), this);
            connect (thread, &ConnectThread::newMsg, this, &QClient::haveNewMsg, Qt::QueuedConnection);
            connect (thread, &ConnectThread::finished, thread, &ConnectThread::deleteLater);
            connect (this, &QClient::msgToSend, thread, &ConnectThread::sendMsg, Qt::QueuedConnection);
            connect (thread, &ConnectThread::loseConnect, this, &QClient::loseConnect, Qt::QueuedConnection);
            connect (thread, &ConnectThread::setUpCommunication, this, &QClient::setUpCommunication, Qt::QueuedConnection);
            threadPool.addThread (thread);
        }
        else {
            Message *msg = new Message(Message::ChatMsg);
            msg->addArgv (ui->msgTextEdit->toHtml ());
            emit msgToSend (thread, msg);
            sendMsgToUI ();
        }
    }
    else {
        Message *msg = new Message(Message::OfflineMsg);
        msg->addArgv (QString::number (currentID));
        msg->addArgv (ui->msgTextEdit->toHtml ());
        emit sendMsg (msg);
        sendMsgToUI ();
    }
}

void QClient::setUpCommunication(ConnectThread* thread)
{
    Message *msg = new Message(Message::HeartbeatMsg);
    msg->addArgv (QString::number (userID));
    msg->addArgv (ui->msgTextEdit->toHtml ());
    emit msgToSend (thread, msg);
    sendMsgToUI ();
}

bool QClient::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->msgTextEdit) {
        if (event->type()==QEvent::KeyPress) {
            QKeyEvent* key = static_cast<QKeyEvent*>(event);
            if ( (key->key()==Qt::Key_Enter) || (key->key()==Qt::Key_Return) ) {
                clickSend ();
            } else {
                return QObject::eventFilter(obj, event);
            }
            return true;
        } else {
            return QObject::eventFilter(obj, event);
        }
        return false;
    }
    else {
        return QObject::eventFilter(obj, event);
    }
}

void QClient::currentFontChanged(QFont f)
{
    ui->msgTextEdit->setCurrentFont (f);
    ui->msgTextEdit->setFocus ();
}

void QClient::currentFontSizeChanged(const QString &size)
{
    ui->msgTextEdit->setFontPointSize (size.toDouble ());
    ui->msgTextEdit->setFocus ();
}

void QClient::clickBoldBtn(bool checked)
{
    if (checked)
        ui->msgTextEdit->setFontWeight (QFont::Bold);
    else
        ui->msgTextEdit->setFontWeight (QFont::Normal);
    ui->msgTextEdit->setFocus ();
}

void QClient::clickItalicBtn(bool checked)
{
    ui->msgTextEdit->setFontItalic (checked);
    ui->msgTextEdit->setFocus ();
}

void QClient::clickUnderlineBtn(bool checked)
{
    ui->msgTextEdit->setFontUnderline (checked);
    ui->msgTextEdit->setFocus ();
}

void QClient::clickColorBtn()
{
    color = QColorDialog::getColor (color, this);
    if (color.isValid ()) {
        ui->msgTextEdit->setTextColor (color);
        ui->msgTextEdit->setFocus ();
    }
}

void QClient::setRedDot(const QString &name)
{
    QList<QTableWidgetItem*> find = ui->userTableWidget->findItems (name, Qt::MatchExactly);
    int row = find.at (0)->row ();
    QTableWidgetItem *item = new QTableWidgetItem(name);
    item->setData (Qt::ForegroundRole, QColor(Qt::red));
    ui->userTableWidget->setItem (row, 0, item);
}


void QClient::haveNewConnect(qintptr socketDescriptor)
{
    ConnectThread* thread = new ConnectThread(socketDescriptor, this);
    connect (thread, &ConnectThread::newMsg, this, &QClient::haveNewMsg, Qt::QueuedConnection);
    connect (thread, &ConnectThread::finished, thread, &ConnectThread::deleteLater);
    connect (this, &QClient::msgToSend, thread, &ConnectThread::sendMsg, Qt::QueuedConnection);
    connect (thread, &ConnectThread::loseConnect, this, &QClient::loseConnect, Qt::QueuedConnection);
    threadPool.addThread (thread);
}

void QClient::haveNewMsg(ConnectThread *thread, Message *msg)
{
    switch (msg->getType ()) {
    case Message::HeartbeatMsg:
        thread->setUserID (static_cast<quint32>(msg->getArgv (0).toInt ()));
        if (thread->getUserID () != currentID) {
            UserInfo *user = userList.getUser (thread->getUserID ());
            user->msgQueue.enqueue (msg->getArgv (1));
            setRedDot(user->getName ());
        }
        else {
            readMsgUpdateUI (userList.getName (thread->getUserID ()), msg->getArgv (1));
        }
        break;
    case Message::ChatMsg:
        if (thread->getUserID () != currentID) {
            UserInfo *user = userList.getUser (thread->getUserID ());
            user->msgQueue.enqueue (msg->getArgv (0));
            setRedDot(user->getName ());
        }
        else {
            readMsgUpdateUI (userList.getName (currentID), msg->getArgv (0));
        }
        break;
    default:
        break;
    }
    delete msg;
}

void QClient::loseConnect(ConnectThread *thread)
{
    threadPool.removeThread (thread);
}
