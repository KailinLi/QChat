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
//    quint32 newSignUp(const QString &name, const QString &password,
//                   const QString &pwQuestion, const QString &pwAnswer);
//    void signIn(quint32 id, quint16 port);
//    void signOut(quint32 id);
//    bool ifPasswordRight(const QString &name, const QString &password);
//    bool checkUserName(const QString &name);
//    UserInfo *findPassword(const QString &name);
};

#endif // USERINFOLIST_H
