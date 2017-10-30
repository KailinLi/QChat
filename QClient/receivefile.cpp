#include "receivefile.h"
#include "ui_receivefile.h"

ReceiveFile::ReceiveFile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReceiveFile)
{
    ui->setupUi(this);
    setAttribute (Qt::WA_DeleteOnClose);
    ui->exitBtn->setEnabled (false);
    connect (ui->receiveBtn, &QPushButton::clicked, this, &ReceiveFile::receiveFile);
    connect (ui->exitBtn, &QPushButton::clicked, this, &ReceiveFile::endReceive);
}

ReceiveFile::~ReceiveFile()
{
    delete ui;
}

void ReceiveFile::initData(const QString &fileName, QFile *file, QHostAddress address, quint16 port,
                           QHostAddress destination, quint16 destinationPort, qint64 fileSize)
{
    this->fileName = fileName;
    this->file = file;
    this->address = address;
    this->port = port;
    this->destination = destination;
    this->destinationPort = destinationPort;
    this->fileSize = fileSize;
}

void ReceiveFile::updateProcess(qint64 t)
{
    float useTime = time.elapsed ();
    ui->speedLabel->setText (tr("已接收 %1MB (%2MB/s)").arg (t / (1024 * 1024))
                             .arg ((static_cast<float_t>(t)/useTime) * 1000 / (1024 * 1024), 0, 'f', 2));
    ui->progressBar->setValue (t);
}

void ReceiveFile::receiveFile()
{
    ui->progressBar->setMaximum (fileSize);
    ui->fileNameLabel->setText (tr("正在接收 %1 ...").arg(fileName));
    thread = new ReceiveFileThread(this);
    thread->receiver->setFile (file, fileSize);
    thread->receiver->setDestination (destination, destinationPort);
    thread->receiver->bindListen (address, port);
    connect (thread, &ReceiveFileThread::updateProcess, this, &ReceiveFile::updateProcess, Qt::QueuedConnection);
    connect (thread, &ReceiveFileThread::finishReceive, this, &ReceiveFile::finishReceive, Qt::QueuedConnection);
//    connect (thread, &ReceiveFileThread::finished, thread, &ReceiveFileThread::deleteLater);
    thread->start();
    time.start ();
}

void ReceiveFile::finishReceive()
{
    qDebug() << "back to UI";
    ui->progressBar->setValue (fileSize);
    ui->fileNameLabel->setText (tr("接收完毕"));
    ui->speedLabel->clear ();
    ui->exitBtn->setEnabled (true);
    ui->receiveBtn->setEnabled (false);
    qDebug() << "UI ok";
}

void ReceiveFile::endReceive()
{
    thread->stop ();
    while(! thread->isFinished ()) {
        qDebug() << "kill";
    }
    thread->deleteLater ();
    qDebug() << "kill all";
    close ();
}
