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

void ReceiveFile::initData(QFile *file, QHostAddress address, quint16 port,
                           QHostAddress destination, quint16 destinationPort, qint64 fileSize)
{
    this->file = file;
    this->address = address;
    this->port = port;
    this->destination = destination;
    this->destinationPort = destinationPort;
    this->fileSize = fileSize;
}

void ReceiveFile::updateProcess(qint64 t)
{
    ui->progressBar->setMaximum (fileSize);
    ui->progressBar->setValue (t);
}

void ReceiveFile::on_pushButton_clicked()
{
    ReceiveFileThread *thread = new ReceiveFileThread(this);
    thread->receiver->setFile (file, fileSize);
    thread->receiver->setDestination (destination, destinationPort);
    thread->receiver->bindListen (address, port);
    connect (thread, &ReceiveFileThread::updateProcess, this, &ReceiveFile::updateProcess, Qt::QueuedConnection);
    connect (thread, &ReceiveFileThread::finished, thread, &ReceiveFileThread::deleteLater);
    thread->start();
}
