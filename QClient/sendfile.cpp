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

void SendFile::initSocket(QFile *file, QHostAddress &destination, quint16 destinationPort, QHostAddress &address, quint16 port)
{
    fileSize = static_cast<quint64>(file->size ());
    thread = new SendFileThread(this, file, destination, destinationPort, address, port);
    connect (thread, &SendFileThread::updateProgress, this, &SendFile::updateProgress, Qt::QueuedConnection);
    thread->start ();
}

void SendFile::updateProgress(int x)
{
    ui->progressBar->setMaximum (fileSize);
    ui->progressBar->setValue (x);
}
