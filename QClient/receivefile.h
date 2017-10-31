#ifndef RECEIVEFILE_H
#define RECEIVEFILE_H

#include <QDialog>
#include "receivefilethread.h"
#include <QTime>

namespace Ui {
class ReceiveFile;
}

class ReceiveFile : public QDialog
{
    Q_OBJECT

public:
    explicit ReceiveFile(QWidget *parent = 0);
    ~ReceiveFile();


private:
    Ui::ReceiveFile *ui;
    QFile *file;
    QHostAddress address;
    quint16 port;
    QHostAddress destination;
    quint16 destinationPort;
    qint64 fileSize;
    QString fileName;
    QTime time;
    ReceiveFileThread *thread;

    float_t saveTime;
    qint64 saveT;
public:
    void initData(const QString &fileName, QFile *file,
                  QHostAddress address, quint16 port,
                  QHostAddress destination, quint16 destinationPort, qint64 fileSize);

public slots:
    void updateProcess(qint64 t);
    void receiveFile();
    void finishReceive();
    void endReceive();
};

#endif // RECEIVEFILE_H
