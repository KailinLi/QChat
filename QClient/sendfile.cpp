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

void SendFile::initData(QFile *file, QHostAddress destination, quint16 destinationPort,
                        QHostAddress address, quint16 port)
{
    this->file = file;
    this->destination = destination;
    this->destinationPort = destinationPort;
    this->address = address;
    this->port = port;
}

void SendFile::updateProcess(qint64 t)
{
    ui->progressBar->setMaximum (file->size ());
    ui->progressBar->setValue (t);
}

void SendFile::on_pushButton_clicked()
{
    SendFileThread *thread = new SendFileThread(this);
    thread->sender->setFile (file);
    thread->sender->setDestination (destination, destinationPort);
    thread->sender->bindListen (address, port);
    connect (thread, &SendFileThread::updateProcess, this, &SendFile::updateProcess, Qt::QueuedConnection);
    thread->start ();
}
