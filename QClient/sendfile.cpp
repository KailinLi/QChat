#include "sendfile.h"
#include "ui_sendfile.h"

SendFile::SendFile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SendFile)
{
    ui->setupUi(this);
    setAttribute (Qt::WA_DeleteOnClose);
    ui->exitBtn->setEnabled (false);
    connect (ui->sendBtn, &QPushButton::clicked, this, &SendFile::sendFile);
    connect (ui->exitBtn, &QPushButton::clicked, this, &SendFile::endSend);
}


SendFile::~SendFile()
{
    delete ui;
}

void SendFile::initData(QString fileName, QFile *file, QHostAddress destination, quint16 destinationPort,
                        QHostAddress address, quint16 port)
{
    this->fileName = fileName;
    this->file = file;
    this->destination = destination;
    this->destinationPort = destinationPort;
    this->address = address;
    this->port = port;
}

void SendFile::updateProcess(qint64 t)
{
//    if (!(t % 24000)) return;
    float useTime = time.elapsed ();
    ui->speedLabel->setText (tr("已发送 %1MB (%2MB/s)").arg (t / (1024 * 1024))
                             .arg ((static_cast<float_t>(t)/useTime) * 1000 / (1024 * 1024), 0, 'f', 2));
//    ui->progressBar->setMaximum (file->size ());
    ui->progressBar->setValue (t);
}

void SendFile::finishSend()
{
    ui->progressBar->setValue (ui->progressBar->maximum ());
    ui->fileNameLabel->setText (tr("发送完毕"));
    ui->speedLabel->clear ();
    ui->exitBtn->setEnabled (true);
    ui->sendBtn->setEnabled (false);
}

void SendFile::sendFile()
{
    ui->progressBar->setMaximum (file->size ());
    ui->fileNameLabel->setText (tr("正在发送 %1 ...").arg(fileName));
    thread = new SendFileThread(this);
    thread->setFile (file);
    thread->setDestination (destination, destinationPort);
    thread->setAddress (address, port);
    connect (thread, &SendFileThread::updateProcess, this, &SendFile::updateProcess, Qt::QueuedConnection);
    connect (thread, &SendFileThread::finishSend, this, &SendFile::finishSend, Qt::QueuedConnection);
//    connect (thread, &SendFileThread::finished, thread, &SendFileThread::deleteLater);
    thread->start ();
    time.start ();
}

void SendFile::endSend()
{
    thread->stop ();
    while(! thread->isFinished ()) {
        qDebug() << "kill";
    }
    thread->deleteLater ();
    qDebug() << "kill all";
    close ();
}
