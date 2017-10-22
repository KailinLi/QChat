#ifndef USERINFOLIST_H
#define USERINFOLIST_H

#include <QObject>
#include "userinfo.h"

class UserInfoList
{
public:
    UserInfoList();

private:
    QList<UserInfo> list;
    int onlineCount;

public:
    void addUser(quint32 id, const QString &name, const QString &address, quint16 port, bool ifOnline);
    QList<UserInfo> *getList();
    void newSignIn(quint32 id, const QString &name, const QString &address, quint16 port);
    int getOnlineCount();
    QString& userSignOut(quint32 id);
    void getActiveInfo(const QString& name, QPair<quint32, bool> &pair);
//    quint32 newSignUp(const QString &name, const QString &password,
//                   const QString &pwQuestion, const QString &pwAnswer);
//    void signIn(quint32 id, quint16 port);
//    void signOut(quint32 id);
//    bool ifPasswordRight(const QString &name, const QString &password);
//    bool checkUserName(const QString &name);
//    UserInfo *findPassword(const QString &name);
};

#endif // USERINFOLIST_H
