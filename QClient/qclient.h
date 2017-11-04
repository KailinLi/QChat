#ifndef QCLIENT_H
#define QCLIENT_H

#include <QWidget>
#include <QTextCharFormat>
#include "connectthreadpool.h"
#include "parallelserver.h"
#include "userinfolist.h"
#include "sendfile.h"
#include "receivefile.h"

namespace Ui {
class QClient;
}

class QClient : public QWidget
{
    Q_OBJECT

public:
    explicit QClient(QWidget *parent = 0);
    ~QClient();

    quint32 userID;
    QHostAddress serverAddress;

private:
    Ui::QClient *ui;
    ParallelServer *server;
    ConnectThreadPool threadPool;
    UserInfoList userList;
    TcpSocketMsg* tcpToServer;
    QString userName;
    quint32 currentID;
    QColor color;
    QFile *sendFile;
    QHostAddress localAddress;

    ReceiveFile *receiveFileWindow;
    SendFile *sendFileWindow;
public:
    void start();
    void logIn();
    void haveNewMsgFromServer();
    void initUpdateUI();
    void signInUpdateUI(const QString &name);
    void signOutUpdateUI(const QString &name);
    void quit();
    void makeActive(int row);
    void changeTableWidget(quint32 id);
    void sendMsgToUI();
    void clickSend();
    void setUpCommunication(ConnectThread *thread);
    bool eventFilter(QObject* obj, QEvent* event);
    void currentFontChanged(QFont f);
    void currentFontSizeChanged(const QString &size);
    void clickBoldBtn(bool checked);
    void clickItalicBtn(bool checked);
    void clickUnderlineBtn(bool checked);
    void clickColorBtn();
    void clickSendFile();
    void clickSaveFile();
    void clickClearBtn();
    void currentFormatChanged(const QTextCharFormat &format);
    void setRedDot(const QString &name);
    void readMsgUpdateUI(const QString &name, const QString &msg);
    QHostAddress getIP();
private slots:
    void haveNewConnect(qintptr socketDescriptor);
    void haveNewMsg(ConnectThread* thread, Message* msg);
    void loseConnect(ConnectThread* thread);
signals:
    void sendMsg(Message* msg);
    void msgToSend(ConnectThread *thread, Message *msg);
};

#endif // QCLIENT_H
