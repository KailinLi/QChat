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
    foreach (UserInfo user, list) {
        if (user.getUserID () == id) {
            user.setIfOnline (true);
            user.setPort (port);
            break;
        }
    }
}

void UserInfoList::signOut(quint32 id)
{
    foreach (UserInfo user, list) {
        if (user.getUserID () == id) {
            user.setPort(0);
            user.setIfOnline (false);
            break;
        }
    }
}

bool UserInfoList::ifPasswordRight(const QString &name, const QString &password)
{
    foreach (UserInfo user, list) {
        if (!QString::compare (user.getName (), name, Qt::CaseSensitive)) {
//            qDebug() << user.getName ();
//            qDebug() << name;
//            qDebug() << user.getPassword ();
//            qDebug() << password;
            if (!QString::compare (user.getPassword (), password, Qt::CaseSensitive)) return true;
            else return false;
        }
    }
    return false;
}
