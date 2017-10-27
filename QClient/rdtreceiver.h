#ifndef RDTRECEIVER_H
#define RDTRECEIVER_H

#include <QObject>
#include <QFile>
#include "rdtreceiverthread.h"

class RdtReceiver : public QUdpSocket
{
    Q_OBJECT
public:
    RdtReceiver(QObject *parent);

private:
    RdtReceiverThread *thread;
    QFile *file;
    qint64 blockSize;
    qint64 bytesHadWritten;
    qint64 sequenceNumber;
    qint64 totalSize;
public:
    void setFile(QFile *file);
    void setTotalSize(qint64 fileSize);
    void initThread(QHostAddress &destination, quint16 destinationPort);
    void readRdtData();
signals:
    void sendACK(qint64);
    void updateProgress(qint64);
};

#endif // RDTRECEIVER_H
