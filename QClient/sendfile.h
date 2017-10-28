#ifndef SENDFILE_H
#define SENDFILE_H

#include <QDialog>
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

private:
    Ui::SendFile *ui;

    QFile *file;
    QHostAddress destination;
    quint16 destinationPort;
    QHostAddress address;
    quint16 port;
public:
    void initData(QFile *file,
                  QHostAddress destination, quint16 destinationPort,
                  QHostAddress address, quint16 port);

public slots:
    void updateProcess(qint64 t);

    void on_pushButton_clicked();
};

#endif // SENDFILE_H
