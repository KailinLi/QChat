#ifndef USERINFO_H
#define USERINFO_H

#include <QObject>
#include "message.h"

class UserInfo
{
public:
    UserInfo(quint32 id, const QString &name, const QString &address, quint16 port, bool ifOnline);
private:
    quint32 userID;
    QString name;
    QString address;
    quint16 port;
    bool ifOnline;
//    QList<Message> offlineMsg;
public:
    quint32 getUserID();
    void setUserID(quint32 id);
    QString &getName();
    void setName(const QString& name);
    QString &getAddress();
    void setAddress(const QString& address);
    quint16 getPort();
    void setPort(quint16 port);
    bool getIfOnline();
    void setIfOnline(bool ifOnline);
};

#endif // USERINFO_H
