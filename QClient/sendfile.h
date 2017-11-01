#ifndef SENDFILE_H
#define SENDFILE_H

#include <QDialog>
#include <QObject>
#include "rdtsender.h"
#include <QTime>

namespace Ui {
class SendFile;
}

class SendFile : public QDialog
{
    Q_OBJECT
public:
    explicit SendFile(QWidget *parent = 0);
    ~SendFile();

private:
    Ui::SendFile *ui;

    QFile *file;
    QHostAddress destination;
    quint16 destinationPort;
    QHostAddress address;
    quint16 port;

    QTime time;
    QString fileName;


//    QThread thread;
//    SendFileThread *thread;
    RdtSender *sender;

    float_t saveTime;
    qint64 saveT;
public:
    void initData(QString fileName, QFile *file,
                  QHostAddress destination, quint16 destinationPort,
                  QHostAddress address, quint16 port);

public slots:
    void updateProcess(qint64 t);
    void finishSend();
    void sendFile();
    void endSend();
signals:
    void startSend();
};

#endif // SENDFILE_H
