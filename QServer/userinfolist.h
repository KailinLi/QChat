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

public:
    quint32 newSignUp(const QString &name, const QString &password,
                   const QString &pwQuestion, const QString &pwAnswer);
    void signIn(quint32 id, quint16 port);
    void signOut(quint32 id);
    quint32 ifPasswordRight(const QString &name, const QString &password);
    bool checkUserName(const QString &name);
    UserInfo *findPassword(const QString &name);
    int size();
    void makeInitMsg(quint32 id, Message *msg);
    QString &newSignIn(quint32 id, const QString &address, quint16 port);
    void userSignOut(quint32 id);
    UserInfo *getUser(quint32 id);
};

#endif // USERINFOLIST_H
