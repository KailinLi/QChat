#ifndef RDTSENDER_H
#define RDTSENDER_H

#include <QObject>
#include <QTimer>
#include "rdtsendersocket.h"

class RdtSender: public QObject
{
    Q_OBJECT
public:
    RdtSender(QFile *file, QHostAddress &destination, quint16 destinationPort);
    ~RdtSender();
//    enum State {
//        Wait,
//        Send,
//        Finish
//    };

//    void startSend();
//    void readRdtACK();
private:
    RdtSenderSocket *sender;
    QUdpSocket *receiver;
    QFile *file;
//    QHostAddress destination;
//    quint16 destinationPort;
//    QHostAddress address;
//    quint16 port;

    qint64 totalSize;
    qint64 baseSize;

    volatile qint64 N;

    volatile qint64 base;
    volatile qint64 nextSeqnum;

//    qint64 bytesNotWrite;

//    QByteArray outBlock;
    QByteArray dataGram;

    QTimer *timer;
    QTimer updateTimer;

    QThread thread;
//    char dataGram[sizeof(qint64)];
public:
    void setFile(QFile *file);
//    void setDestination(QHostAddress &destination, quint16 destinationPort);
    void startSend();
    void readRdtACK();
    void bindListen(QHostAddress &address, quint16 port);
    void timeOut();
    void canUpdate();
//    void startTheTimer();
//    void sendFilePiece();
//    void setFile(QFile *file);
signals:
//    void canSend();
    void deleteSender();
    void finish();
    void updateProgress(qint64);
    void sendFile(volatile qint64 *base, volatile qint64 *nextSeqnum, volatile qint64 *N);
    void resendFile(volatile qint64 *base, volatile qint64 *nextSeqnum, volatile qint64 *N);
};

#endif // RDTSENDER_H
