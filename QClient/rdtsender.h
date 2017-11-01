#ifndef RDTSENDER_H
#define RDTSENDER_H

#include <QObject>
#include <QtNetwork>
#include <QTimer>

class RdtSender : public QUdpSocket
{
    Q_OBJECT
public:
    RdtSender(QObject *parent = Q_NULLPTR);
//    enum State {
//        Wait,
//        Send,
//        Finish
//    };

    void startSend();
    void sendFilePiece();
    void readRdtACK();
private:
    QUdpSocket *receiver;
    QFile *file;
    QHostAddress destination;
    quint16 destinationPort;

    qint64 totalSize;

    qint64 base;
    qint64 nextSeqnum;

//    qint64 bytesNotWrite;

    QByteArray outBlock;
    QByteArray dataGram;

    QTimer *timer;
//    char dataGram[sizeof(qint64)];
public:
    void setFile(QFile *file);
    void setDestination(QHostAddress &destination, quint16 destinationPort);
    void bindListen(QHostAddress &address, quint16 port);
    void timeOut();
    void startTheTimer();
signals:
    void canSend();
//    void finish();
    void updateProgress(qint64);
    void timerStartSignal();
    void timerStopSignal();
};

#endif // RDTSENDER_H
