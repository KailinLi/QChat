#ifndef MESSAGE_H
#define MESSAGE_H

#include <QObject>

class Message
{
public:
    enum Type : int {
        SignIn,
        AnswerSignIn,
        SignUp,
        InitMsg,
        FinishInit,
        UpdateMsg
    };
private:
    Message::Type type;
    QStringList argv;
public:
    Message(Message::Type t);
    Message::Type getType();
    void addArgv(QString &s);
    QString getArgv(int i);
    int argc();
};

#endif // MESSAGE_H
