#include "message.h"


Message::Message(Message::Type t):
    type(t)
{

}

Message::Type Message::getType()
{
    return type;
}

void Message::addArgv(const QString &s)
{
    argv.push_back (s);
}

QString Message::getArgv(int i)
{
    return argv[i];
}

int Message::argc()
{
    return argv.size ();
}
