#ifndef RDTSENDER_H
#define RDTSENDER_H

#include <QObject>
#include <QtNetwork>

class RdtSender : public QUdpSocket
{
    Q_OBJECT
public:
    RdtSender(QObject *parent = Q_NULLPTR);
    enum State {
        Wait,
        Send,
        Finish
    };

//    void setSender(const QHostAddress &destination, quint16 destinationPort);
//    void setRcv(const QHostAddress &destination, quint16 destinationPort);
    void startSend();
    void sendFilePiece();
    void readRdtACK();
private:
    QUdpSocket *receiver;
    QFile *file;
    QHostAddress destination;
    quint16 destinationPort;
    State state;
//    int data;
//    int current;
//    qint64 sendSize;
    qint64 totalSize;
    qint64 bytesHadWritten;
    qint64 bytesNotWrite;

    QByteArray outBlock;
    QByteArray dataGram;
//    char dataGram[sizeof(qint64)];
public:
    void setFile(QFile *file);
    void setDestination(QHostAddress &destination, quint16 destinationPort);
    void bindListen(QHostAddress &address, quint16 port);
signals:
    void canSend();
//    void finish();
    void updateProgress(qint64);
};

#endif // RDTSENDER_H
