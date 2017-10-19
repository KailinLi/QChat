#ifndef MESSAGE_H
#define MESSAGE_H

#include <QObject>

class Message
{
public:
    enum Type {
        SignIn,
        AnswerSignIn,
        SignUp,
        InitMsg,
        FinishInit,
        UpdateMsg
    };
private:
    Type type;
    QStringList argv;
public:
    Message(Type t);
    Message::Type getType();
    void addArgv(QString &s);
    QString getArgv(int i);
    int argc();
};

#endif // MESSAGE_H
