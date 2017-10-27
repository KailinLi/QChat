#include "sendfile.h"
#include "ui_sendfile.h"

SendFile::SendFile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SendFile)
{
    ui->setupUi(this);
}

SendFile::~SendFile()
{
    delete ui;
}

void SendFile::updateProcess(qint64 t)
{
    ui->progressBar->setMaximum (10000);
    ui->progressBar->setValue (t);
}

void SendFile::on_pushButton_clicked()
{
    SendFileThread *thread = new SendFileThread(this);
    connect (thread, &SendFileThread::updateProcess, this, &SendFile::updateProcess);
    thread->start ();
}
