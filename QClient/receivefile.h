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

private slots:
    void updateProcess(qint64 t);
    void on_pushButton_clicked();

private:
    Ui::ReceiveFile *ui;
};

#endif // RECEIVEFILE_H
