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

private slots:
    void updateProcess(qint64 t);

    void on_pushButton_clicked();

private:
    Ui::SendFile *ui;
};

#endif // SENDFILE_H
