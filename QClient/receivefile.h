#ifndef RECEIVEFILE_H
#define RECEIVEFILE_H

#include <QDialog>
#include "receivefilethread.h"

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

public:
    void initData(QFile *file,
                  QHostAddress address, quint16 port,
                  QHostAddress destination, quint16 destinationPort, qint64 fileSize);

public slots:
    void updateProcess(qint64 t);
    void on_pushButton_clicked();
};

#endif // RECEIVEFILE_H
