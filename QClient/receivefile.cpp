#include "receivefile.h"
#include "ui_receivefile.h"

ReceiveFile::ReceiveFile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReceiveFile)
{
    ui->setupUi(this);
    setAttribute (Qt::WA_DeleteOnClose);
}

ReceiveFile::~ReceiveFile()
{
    delete ui;
}

void ReceiveFile::initSocket(QFile *file,
                             QHostAddress address, quint16 port,
                             QHostAddress destination, quint16 destinationPort,
                             qint64 fileSize)
{
    this->fileSize = fileSize;
    thread = new ReceiveFileThread(this);
    thread->init (file, address, port, destination, destinationPort, fileSize);
    connect (thread, &ReceiveFileThread::updateProgress, this, &ReceiveFile::updateProgress, Qt::QueuedConnection);
    thread->start ();
}

void ReceiveFile::updateProgress(qint64 x)
{
    ui->progressBar->setMaximum (fileSize);
    ui->progressBar->setValue (x);
}
