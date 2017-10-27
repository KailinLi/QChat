#include "receivefile.h"
#include "ui_receivefile.h"

ReceiveFile::ReceiveFile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReceiveFile)
{
    ui->setupUi(this);
}

ReceiveFile::~ReceiveFile()
{
    delete ui;
}

void ReceiveFile::updateProcess(qint64 t)
{
    ui->progressBar->setMaximum (10000);
    ui->progressBar->setValue (t);
}

void ReceiveFile::on_pushButton_clicked()
{
    ReceiveFileThread *thread = new ReceiveFileThread(this);
    connect (thread, &ReceiveFileThread::updateProcess, this, &ReceiveFile::updateProcess);
    thread->start();
}
