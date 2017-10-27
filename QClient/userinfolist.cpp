#include "userinfolist.h"

UserInfoList::UserInfoList():
    onlineCount(0)
{

}

void UserInfoList::addUser(quint32 id, const QString &name, const QString &address, quint16 port, bool ifOnline)
{
    if (ifOnline) ++onlineCount;
    list.push_back (UserInfo(id, name, address, port, ifOnline));
}

QList<UserInfo> *UserInfoList::getList()
{
    return &list;
}

void UserInfoList::newSignIn(quint32 id, const QString &name, const QString &address, quint16 port)
{
    ++onlineCount;
    for (QList<UserInfo>::Iterator user = list.begin (); user != list.end(); ++user) {
        if (user->getUserID () == id) {
            user->setAddress (address);
            user->setPort (port);
            user->setIfOnline (true);
            return;
        }
    }
//    foreach (UserInfo user, list) {
//        if (user.getUserID () == id) {
//            user.setAddress (address);
//            user.setPort (port);
//            user.setIfOnline (true);
//            return;
//        }
//    }
    list.push_back (UserInfo(id, name, address, port, true));
}

int UserInfoList::getOnlineCount()
{
    return onlineCount;
}

QString& UserInfoList::userSignOut(quint32 id)
{
    --onlineCount;
    for (QList<UserInfo>::Iterator user = list.begin (); user != list.end(); ++user) {
        if (user->getUserID () == id) {
            user->setAddress (QString());
            user->setPort (0);
            user->setIfOnline (false);
            return user->getName();
        }
    }
    return list.begin ()->getName ();
}


void UserInfoList::getActiveInfo(const QString &name, QPair<quint32, bool> &pair)
{
    for (QList<UserInfo>::Iterator user = list.begin (); user != list.end(); ++user) {
        if (!QString::compare (user->getName(), name)) {
            pair.first = user->getUserID();
            pair.second = user->getIfOnline();
            return;
        }
    }
}

void UserInfoList::saveMsg(quint32 id, const QString &msg)
{
    for (QList<UserInfo>::Iterator user = list.begin (); user != list.end(); ++user) {
        if (user->getUserID () == id) {
            user->setShowMsg(msg);
            break;
        }
    }
}

QString &UserInfoList::getMsg(quint32 id)
{
    for (QList<UserInfo>::Iterator user = list.begin (); user != list.end(); ++user) {
        if (user->getUserID () == id) {
            return user->getShowMsg();
        }
    }
    return list.begin ()->getShowMsg();
}

UserInfo *UserInfoList::getUser(quint32 id)
{
    for (QList<UserInfo>::Iterator user = list.begin (); user != list.end(); ++user) {
        if (user->getUserID () == id) {
            return &(*user);
        }
    }
    return &(*list.begin ());
}

QString &UserInfoList::getName(quint32 id)
{
    for (QList<UserInfo>::Iterator user = list.begin (); user != list.end(); ++user) {
        if (user->getUserID () == id) {
            return user->getName ();
        }
    }
    return list.begin ()->getName ();
}

void UserInfoList::getNetworkInfo(quint32 id, QPair<QString, quint16> &info)
{
    for (QList<UserInfo>::Iterator user = list.begin (); user != list.end(); ++user) {
        if (user->getUserID () == id) {
            info.first = user->getAddress();
            info.second = user->getPort();
            return;
        }
    }
}

//quint32 UserInfoList::newSignUp(const QString &name, const QString &password, const QString &pwQuestion, const QString &pwAnswer)
//{
//    quint32 newID = (quint32)list.size ();
//    list.push_back (UserInfo(++newID, name, password, pwQuestion, pwAnswer));
//    return newID;
//}

//void UserInfoList::signIn(quint32 id, quint16 port)
//{
//    foreach (UserInfo user, list) {
//        if (user.getUserID () == id) {
//            user.setIfOnline (true);
//            user.setPort (port);
//            break;
//        }
//    }
//}

//void UserInfoList::signOut(quint32 id)
//{
//    foreach (UserInfo user, list) {
//        if (user.getUserID () == id) {
//            user.setPort(0);
//            user.setIfOnline (false);
//            break;
//        }
//    }
//}

//bool UserInfoList::ifPasswordRight(const QString &name, const QString &password)
//{
//    foreach (UserInfo user, list) {
//        if (!QString::compare (user.getName (), name, Qt::CaseSensitive)) {
//            if (!QString::compare (user.getPassword (), password, Qt::CaseSensitive)) return true;
//            else return false;
//        }
//    }
//    return false;
//}

//bool UserInfoList::checkUserName(const QString &name)
//{
//    foreach (UserInfo user, list) {
//        if (!QString::compare (user.getName (), name)) {
//            return false;
//        }
//    }
//    return true;
//}

//UserInfo *UserInfoList::findPassword(const QString &name)
//{
//    for (int i = 0; i < list.size (); ++i) {
//        if (!QString::compare (list[i].getName (), name)) {
//            return &(list[i]);
//        }
//    }
//    return nullptr;
//}
