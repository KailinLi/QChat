#include "userinfo.h"


UserInfo::UserInfo(quint32 id, const QString &name, const QString &address, quint16 port, bool ifOnline):
    userID(id),
    name(name),
    address(address),
    port(port),
    ifOnline(ifOnline)
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

QString &UserInfo::getAddress()
{
    return address;
}

void UserInfo::setAddress(const QString &address)
{
    this->address = address;
}

quint16 UserInfo::getPort()
{
    return port;
}

void UserInfo::setPort(quint16 port)
{
    this->port = port;
}

bool UserInfo::getIfOnline()
{
    return ifOnline;
}

void UserInfo::setIfOnline(bool ifOnline)
{
    this->ifOnline = ifOnline;
}
