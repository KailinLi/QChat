#ifndef QCLIENT_H
#define QCLIENT_H

#include <QWidget>
#include "connectthreadpool.h"
#include "parallelserver.h"
#include "userinfolist.h"

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

private:
    Ui::QClient *ui;
    ParallelServer *server;
    ConnectThreadPool threadPool;
    UserInfoList userList;
    TcpSocketMsg* tcpToServer;
    QString userName;
    quint32 currentID;
    QColor color;
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
    void currentFontSizeChanged(QString &size);
    void clickBoldBtn(bool checked);
    void clickItalicBtn(bool checked);
    void clickUnderlineBtn(bool checked);
    void clickColorBtn();
    void setRedDot(const QString &name);
private slots:
    void haveNewConnect(qintptr socketDescriptor);
    void haveNewMsg(ConnectThread* thread, Message* msg);
    void loseConnect(ConnectThread* thread);
signals:
    void sendMsg(Message* msg);
    void msgToSend(ConnectThread *thread, Message *msg);
};

#endif // QCLIENT_H
