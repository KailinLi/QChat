#ifndef RDTSENDER_H
#define RDTSENDER_H

#include <QObject>
#include <QtNetwork>

class RdtSender : public QUdpSocket
{
public:
    RdtSender();
};

#endif // RDTSENDER_H
