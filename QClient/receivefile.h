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

    void initSocket(QFile *file, QHostAddress &destination, quint16 destinationPort, QHostAddress &address, quint16 port);

private:
    Ui::ReceiveFile *ui;

    quint64 fileSize;
    ReceiveFileThread *thread;

    void updateProgress(int x);
};

#endif // RECEIVEFILE_H
