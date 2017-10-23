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
        AnswerSignUp,
        ForgetPassword,
        AnswerForgetPassword,
        LogIn,
        InitMsg,
        FinishInit,
        UpdateMsg,
        ChatMsg,
        HeartbeatMsg,
        OfflineMsg
    };
private:
    Message::Type type;
    QStringList argv;
public:
    Message(Message::Type t);
    Message::Type getType();
    void addArgv(const QString &s);
    QString getArgv(int i);
    int argc();
};

#endif // MESSAGE_H
