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

void ReceiveFile::initSocket(QFile *file,
                             QHostAddress &address, quint16 port,
                             QHostAddress &destination, quint16 destinationPort)
{
    fileSize = static_cast<qint64>(file->size ());
    thread = new ReceiveFileThread(this, file, address, port, destination, destinationPort);
    connect (thread, &ReceiveFileThread::updateProgress, this, &ReceiveFile::updateProgress, Qt::QueuedConnection);
    thread->start ();
}

void ReceiveFile::updateProgress(int x)
{
    ui->progressBar->setMaximum (fileSize);
    ui->progressBar->setValue (x);
}
