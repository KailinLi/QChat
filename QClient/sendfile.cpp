#include "sendfile.h"
#include "ui_sendfile.h"

SendFile::SendFile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SendFile)
{
    ui->setupUi(this);
    setAttribute (Qt::WA_DeleteOnClose);
}

SendFile::~SendFile()
{
    delete ui;
}

void SendFile::initSocket(QFile *file,
                          QHostAddress destination, quint16 destinationPort,
                          QHostAddress address, quint16 port)
{
    fileSize = file->size ();
    thread = new SendFileThread(this);
    thread->init (file, destination, destinationPort, address, port);
    connect (thread, &SendFileThread::updateProgress, this, &SendFile::updateProgress, Qt::QueuedConnection);
    thread->start ();
}

void SendFile::updateProgress(qint64 x)
{
    ui->progressBar->setMaximum (fileSize);
    ui->progressBar->setValue (x);
}
