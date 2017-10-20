#include "userinfo.h"


UserInfo::UserInfo(quint32 id, const QString &name, const QString &password, const QString &pwQuestion, const QString &pwAnswer):
    userID(id),
    name(name),
    password(password),
    pwQuestion(pwQuestion),
    pwAnswer(pwAnswer),
    port(0),
    ifOnline(false)
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

QString &UserInfo::getPassword()
{
    return password;
}

void UserInfo::setPassword(const QString &password)
{
    this->password = password;
}

QString &UserInfo::getPwQuestion()
{
    return pwQuestion;
}

void UserInfo::setPwQuestion(const QString &pwQuestion)
{
    this->pwQuestion = pwQuestion;
}

QString &UserInfo::getPwAnswer()
{
    return pwAnswer;
}

void UserInfo::setPwAnswer(const QString &pwAnswer)
{
    this->pwAnswer = pwAnswer;
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
