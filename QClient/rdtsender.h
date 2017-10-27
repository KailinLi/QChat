#ifndef RDTSENDER_H
#define RDTSENDER_H

#include <QObject>
#include <QtNetwork>

class RdtSender : public QUdpSocket
{
    Q_OBJECT
public:
    RdtSender(QObject *parent);
    enum State {
        Wait,
        Send,
        Finish
    };

    void setSender(const QHostAddress &address, quint16 port);
    void setRcv(const QHostAddress &address, quint16 port);
    void sendData();
    void sendPiece();
    void rdtRcv();
private:
    QUdpSocket *rcvSocket;
    int data;
    int current;
    QHostAddress address;
    quint16 port;
    State state;
signals:
    void canSend();
    void finish();
    void step(qint64);
};

#endif // RDTSENDER_H
