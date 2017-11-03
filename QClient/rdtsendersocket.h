#ifndef RDTSENDERSOCKET_H
#define RDTSENDERSOCKET_H

#include <QObject>
#include <QtNetwork>

class RdtSenderSocket : public QUdpSocket
{
    Q_OBJECT
public:
    RdtSenderSocket(QObject *parent = 0, QFile *file = nullptr,
                    QHostAddress destination = QHostAddress(), quint16 destinationPort = 0, qint64 totalSize = 0);
private:
    QFile *file;
    QByteArray outBlock;
    qint64 totalSize;
public:
    void sendFile(volatile qint64 *base, volatile qint64 *nextSeqnum, volatile qint64 *N);
    void timeOut(volatile qint64 *base, volatile qint64 *nextSeqnum, volatile qint64 *N);
    void deleteSelf();
//    QHostAddress destination;
//    quint16 destinationPort;
};

#endif // RDTSENDERSOCKET_H
