#include "userinfolist.h"

UserInfoList::UserInfoList()
{

}

quint32 UserInfoList::newSignUp(const QString &name, const QString &password, const QString &pwQuestion, const QString &pwAnswer)
{
    quint32 newID = (quint32)list.size ();
    list.push_back (UserInfo(++newID, name, password, pwQuestion, pwAnswer));
    return newID;
}

void UserInfoList::signIn(quint32 id, quint16 port)
{
//    QMutableListIterator<UserInfo> user(list);
//    while (user.hasNext ()) {
//        if (user.next ().getUserID() == id) {
//            user.next ().setIfOnline (true);
//            user.next ().setPort (port);
//            break;
//        }
//    }
    for (QList<UserInfo>::Iterator user = list.begin (); user != list.end (); ++user) {
        if (user->getUserID() == id) {
            user->setIfOnline (true);
            user->setPort (port);
            break;
        }
    }
//    foreach (UserInfo user, list) {
//        if (user.getUserID () == id) {
//            user.setIfOnline (true);
//            user.setPort (port);
//            break;
//        }
//    }
}

void UserInfoList::signOut(quint32 id)
{
//    QMutableListIterator<UserInfo> user(list);
//    while (user.hasNext ()) {
//        if (user.next ().getUserID () == id) {
//            user.next ().setPort (0);
//            user.next ().setIfOnline (false);
//            break;
//        }
//    }
    for (QList<UserInfo>::Iterator user = list.begin (); user != list.end (); ++user) {
        if (user->getUserID() == id) {
            user->setIfOnline (false);
            user->setPort (0);
            break;
        }
    }
//    foreach (UserInfo user, list) {
//        if (user.getUserID () == id) {
//            user.setPort(0);
//            user.setIfOnline (false);
//            break;
//        }
//    }
}

quint32 UserInfoList::ifPasswordRight(const QString &name, const QString &password)
{
    foreach (UserInfo user, list) {
        if (!QString::compare (user.getName (), name, Qt::CaseSensitive)) {
//            qDebug() << user.getName ();
//            qDebug() << name;
//            qDebug() << user.getPassword ();
//            qDebug() << password;
            if (!QString::compare (user.getPassword (), password, Qt::CaseSensitive)) return user.getUserID ();
            else return 0;
        }
    }
    return 0;
}

bool UserInfoList::checkUserName(const QString &name)
{
    foreach (UserInfo user, list) {
        if (!QString::compare (user.getName (), name)) {
            return false;
        }
    }
    return true;
}

UserInfo *UserInfoList::findPassword(const QString &name)
{
    for (int i = 0; i < list.size (); ++i) {
        if (!QString::compare (list[i].getName (), name)) {
            return &(list[i]);
        }
    }
    return nullptr;
}

int UserInfoList::size()
{
    return list.size ();
}

void UserInfoList::makeInitMsg(quint32 id, Message *msg)
{
    msg->addArgv (QString::number (list.size ()));
    foreach (UserInfo user, list) {
        msg->addArgv (QString::number (user.getUserID ()));
        msg->addArgv (user.getName ());
        msg->addArgv (user.getAddress ());
        msg->addArgv (QString::number (user.getPort ()));
        if (user.getIfOnline ()) msg->addArgv (QObject::tr("y"));
        else msg->addArgv (QObject::tr("n"));
    }
    UserInfo* user = getUser (id);
    msg->addArgv (QString::number (user->msgQueue.size ()));
    while (!user->msgQueue.isEmpty ()) {

    }
}

QString& UserInfoList::newSignIn(quint32 id, const QString &address, quint16 port)
{
    for (QList<UserInfo>::Iterator user = list.begin (); user != list.end (); ++user) {
        if (user->getUserID() == id) {
            user->setAddress (address);
            user->setIfOnline (true);
            user->setPort (port);
            return user->getName ();
        }
    }
//    foreach (UserInfo user, list) {
//        if (user.getUserID () == id) {
//            user.setAddress (address);
//            user.setPort (port);
//            user.setIfOnline (true);
//            return user.getName ();
//        }
//    }
    return const_cast<QString&>(address);
}

void UserInfoList::userSignOut(quint32 id)
{
    for (QList<UserInfo>::Iterator user = list.begin (); user != list.end (); ++user) {
        if (user->getUserID() == id) {
            user->setAddress(QString());
            user->setPort(0);
            user->setIfOnline(false);
            break;
        }
    }
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
