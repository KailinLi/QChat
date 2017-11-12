#include "qclient.h"
#include "ui_qclient.h"
#include <QMessageBox>
#include <QKeyEvent>
#include <QColorDialog>
#include <QSet>
#include <QScrollBar>
#include <QTextBlock>
#include <QFileDialog>
#include "handledir.cpp"

QClient::QClient(QWidget *parent) :
    QWidget(parent),
    serverAddress(QHostAddress("127.0.0.1")),
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
    if (! server->listen (getIP ())) {
        QMessageBox::critical(this, tr("Fail Listening"),
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
    connect (ui->sendToolBtn, &QPushButton::clicked, this, &QClient::clickSendFile);
    connect (ui->saveToolBtn, &QToolButton::clicked, this, &QClient::clickSaveFile);
    connect (ui->clearToolBtn, &QToolButton::clicked, this, &QClient::clickClearBtn);
    connect (ui->msgTextEdit, &QTextEdit::currentCharFormatChanged, this, &QClient::currentFormatChanged);
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
//    qDebug() << userID;
    tcpToServer->setBlockSize (0);
    tcpToServer->connectToHost (serverAddress, 16666);
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
        newMsg->addArgv (localAddress.toString ());
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
    ui->msgBrowser->setTextColor (QColor(93, 94, 95));
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
    ui->msgBrowser->setTextColor (QColor(93, 94, 95));
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

QHostAddress QClient::getIP()
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

void QClient::shakeWindow()
{
    QPropertyAnimation *pAnimation = new QPropertyAnimation(this, "pos");
    pAnimation->setDuration(500);
    pAnimation->setLoopCount(2);
    pAnimation->setKeyValueAt(0, QPoint(geometry().x() - 3, geometry().y() - 3));
    pAnimation->setKeyValueAt(0.1, QPoint(geometry().x() + 6, geometry().y() + 6));
    pAnimation->setKeyValueAt(0.2, QPoint(geometry().x() - 6, geometry().y() + 6));
    pAnimation->setKeyValueAt(0.3, QPoint(geometry().x() + 6, geometry().y() - 6));
    pAnimation->setKeyValueAt(0.4, QPoint(geometry().x() - 6, geometry().y() - 6));
    pAnimation->setKeyValueAt(0.5, QPoint(geometry().x() + 6, geometry().y() + 6));
    pAnimation->setKeyValueAt(0.6, QPoint(geometry().x() - 6, geometry().y() + 6));
    pAnimation->setKeyValueAt(0.7, QPoint(geometry().x() + 6, geometry().y() - 6));
    pAnimation->setKeyValueAt(0.8, QPoint(geometry().x() - 6, geometry().y() - 6));
    pAnimation->setKeyValueAt(0.9, QPoint(geometry().x() + 6, geometry().y() + 6));
    pAnimation->setKeyValueAt(1, QPoint(geometry().x() - 3, geometry().y() - 3));
    pAnimation->start(QAbstractAnimation::DeleteWhenStopped);
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

void QClient::clickSendFile()
{
    QString fileName = QFileDialog::getOpenFileName (this);
    if (!fileName.isEmpty ()) {
        Message *msg = new Message(Message::SendFileMsg);
        sendFile = new QFile(fileName);
        if (!sendFile->open (QFile::ReadOnly)) {
            QMessageBox::warning (this, tr("发送文件"), tr("无法读取文件"));
            return;
        }
        QString currentFileName = fileName.right (fileName.size () - fileName.lastIndexOf ('/') - 1);
        msg->addArgv (currentFileName);
        msg->addArgv (QString::number (sendFile->size ()));
        ConnectThread *thread = threadPool.getThread (currentID);
        emit msgToSend (thread, msg);
    }
}

void QClient::clickSaveFile()
{
    if (ui->msgBrowser->document ()->isEmpty ()) {
        QMessageBox::warning (0, tr("对不起"), tr("聊天记录为空哦!"), QMessageBox::Ok);
    }
    else {
        QString saveFileName = QFileDialog::getSaveFileName (this, tr("保存聊天记录"), tr("聊天记录"), tr("文本(*.txt);;All File(*.*)"));
        if (!saveFileName.isEmpty ()) {
            QFile saveMsgFile(saveFileName);
            if (! saveMsgFile.open (QFile::WriteOnly | QFile::Text)) {
                QMessageBox::warning (this, tr("保存聊天记录"), tr("保存文件失败 %1:\n%2").arg (saveFileName).arg (saveMsgFile.errorString ()));
            }
            QTextStream out(&saveMsgFile);
            out << ui->msgBrowser->toPlainText ();
        }
    }
}

void QClient::clickClearBtn()
{
    userList.saveMsg (currentID, ui->msgBrowser->toHtml ());
    ui->msgBrowser->clear ();
}

void QClient::currentFormatChanged(const QTextCharFormat &format)
{
    ui->fontComboBox->setCurrentFont (format.font ());
    if (format.fontPointSize () < 12) {
        ui->sizeComboBox->setCurrentIndex (4);
    }
    else {
        ui->sizeComboBox->setCurrentIndex (ui->sizeComboBox->findText (QString::number (format.fontPointSize ())));
    }
    ui->boldToolBtn->setChecked (format.font ().bold ());
    ui->italicToolBtn->setChecked (format.font ().italic ());
    ui->underlineToolBtn->setChecked (format.font ().underline ());
    color = format.foreground ().color ();
}

void QClient::setRedDot(const QString &name)
{
    QList<QTableWidgetItem*> find = ui->userTableWidget->findItems (name, Qt::MatchExactly);
    int row = find.at (0)->row ();
    QTableWidgetItem *item = new QTableWidgetItem(name);
    item->setData (Qt::ForegroundRole, QColor(Qt::red));
    ui->userTableWidget->setItem (row, 0, item);
    shakeWindow ();
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
    case Message::SendFileMsg: {
        int choose = QMessageBox::question (this, tr("新文件可接受"), tr("希望接收%1吗?").arg (msg->getArgv (0)),
                                            QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
        if (choose == QMessageBox::No) {
            Message *newMsg = new Message(Message::AnswerSendFileMsg);
            newMsg->addArgv (tr("n"));
            emit msgToSend (thread, newMsg);
        }
        else if (choose == QMessageBox::Yes) {
            QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                            "/home",
                                                            QFileDialog::ShowDirsOnly
                                                            | QFileDialog::DontResolveSymlinks);
            QFile *file = new QFile(dir.append (tr("/%1").arg (msg->getArgv (0))));


            QMessageBox::information (this, tr("文件储存路径"), tr("%1").arg (dir), QMessageBox::Ok);


            if (!file->open (QIODevice::WriteOnly | QIODevice::Truncate)) {
                qDebug() << "can not open file";
                return;
            }
            QPair<QString, quint16> info;
            userList.getNetworkInfo (thread->getUserID (), info);
            receiveFileWindow = new ReceiveFile(this);
            receiveFileWindow->initData (msg->getArgv (0), file, localAddress, server->serverPort (),
                                        QHostAddress(info.first), info.second,
                                        static_cast<qint64>(msg->getArgv (1).toInt ()));

            Message *newMsg = new Message(Message::AnswerSendFileMsg);
            newMsg->addArgv (msg->getArgv (0));
            emit msgToSend (thread, newMsg);
            receiveFileWindow->setModal (false);
            receiveFileWindow->show ();
        }
        break;
    }
    case Message::AnswerSendFileMsg: {
        if (!QString::compare (msg->getArgv (0), tr("n"))) {
            sendFile->close ();
            sendFile->deleteLater ();
            QMessageBox::information (this, tr("发送文件"), tr("对方拒绝接收文件"), QMessageBox::Ok);
        }
        else {
            QPair<QString, quint16> info;
            userList.getNetworkInfo (thread->getUserID (), info);
            sendFileWindow = new SendFile(this);
            sendFileWindow->initData (msg->getArgv (0), sendFile, QHostAddress(info.first), info.second,
                                     localAddress, server->serverPort ());//server->serverPort ());
//            sendFileWindow.initSocket (sendFile, QHostAddress(info.first), info.second,
//                                       localAddress, server->serverPort ());
            sendFileWindow->setModal (false);
            sendFileWindow->show ();
        }
    }
    default:
        break;
    }
    delete msg;
}

void QClient::loseConnect(ConnectThread *thread)
{
    threadPool.removeThread (thread);
}
