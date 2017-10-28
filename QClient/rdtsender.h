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

//    void setSender(const QHostAddress &destination, quint16 destinationPort);
//    void setRcv(const QHostAddress &destination, quint16 destinationPort);
    void sendData();
    void sendPiece();
    void rdtRcv();
private:
    QUdpSocket *receiver;
    QFile *file;
    QHostAddress destination;
    quint16 destinationPort;
    State state;
    int data;
    int current;
public:
    void setFile(QFile *file);
    void setDestination(QHostAddress &destination, quint16 destinationPort);
    void bindListen(QHostAddress &address, quint16 port);
signals:
    void canSend();
    void finish();
    void updateProgress(qint64);
};

#endif // RDTSENDER_H
