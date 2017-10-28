#ifndef RDTRECEIVER_H
#define RDTRECEIVER_H

#include <QObject>
#include <QtNetwork>

class RdtReceiver : public QUdpSocket
{
    Q_OBJECT
public:
    RdtReceiver(QObject *parent = Q_NULLPTR);
private:
    int hasRcv;
    QUdpSocket *sender;
//    QUdpSocket* rcvSocket;
    QFile *file;
    QHostAddress destination;
    quint16 destinationPort;
    qint64 blockSize;
    qint64 bytesHadWritten;
    qint64 sequenceNumber;
    qint64 totalSize;

    QByteArray block;
    QByteArray dataGram;
public:
    void readRdtData();
    void setFile(QFile *file, qint64 fileSize);
    void setDestination(QHostAddress &destination, quint16 destinationPort);
    void bindListen(QHostAddress &address, quint16 port);
    void sendACK(qint64 sequenceNumber);
signals:
    void callACK(qint64);
    void finish();
    void updateProgress(qint64);
};

#endif // RDTRECEIVER_H
