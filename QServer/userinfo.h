#ifndef USERINFO_H
#define USERINFO_H

#include <QObject>
#include "message.h"

class UserInfo
{
public:
    UserInfo(quint32 id, const QString &name, const QString &password,
             const QString &pwQuestion, const QString &pwAnswer);
private:
    quint32 userID;
    QString name;
    QString password;
    QString pwQuestion;
    QString pwAnswer;
    QString address;
    quint16 port;
    bool ifOnline;
//    QList<Message> offlineMsg;
public:
    quint32 getUserID();
    void setUserID(quint32 id);
    QString &getName();
    void setName(const QString& name);
    QString &getPassword();
    void setPassword(const QString& password);
    QString &getPwQuestion();
    void setPwQuestion(const QString& pwQuestion);
    QString &getPwAnswer();
    void setPwAnswer(const QString& pwAnswer);
    QString &getAddress();
    void setAddress(const QString& address);
    quint16 getPort();
    void setPort(quint16 port);
    bool getIfOnline();
    void setIfOnline(bool ifOnline);
};

#endif // USERINFO_H
