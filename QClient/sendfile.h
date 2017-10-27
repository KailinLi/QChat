#ifndef SENDFILE_H
#define SENDFILE_H

#include <QDialog>
#include <QFile>
#include "sendfilethread.h"

namespace Ui {
class SendFile;
}

class SendFile : public QDialog
{
    Q_OBJECT

public:
    explicit SendFile(QWidget *parent = 0);
    ~SendFile();

    void initSocket(QFile *file, QHostAddress &destination, quint16 destinationPort, QHostAddress &address, quint16 port);

private:
    Ui::SendFile *ui;
    quint64 fileSize;
    SendFileThread *thread;

    void updateProgress(int x);
};

#endif // SENDFILE_H
