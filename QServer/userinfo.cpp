#include "userinfo.h"

UserInfo::UserInfo()
{

}

quint32 UserInfo::getUserID()
{
    return userID;
}

void UserInfo::setUserID(quint32 id)
{
    this->userID = id;
}

QString &UserInfo::getName()
{
    return name;
}

void UserInfo::setName(const QString &name)
{
    this->name = name;
}
