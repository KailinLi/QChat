#include "sendfile.h"
#include "ui_sendfile.h"

SendFile::SendFile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SendFile),
    saveTime(0),
    saveT(0)
{
    ui->setupUi(this);
    setAttribute (Qt::WA_DeleteOnClose);
    ui->exitBtn->setEnabled (false);
    connect (ui->sendBtn, &QPushButton::clicked, this, &SendFile::sendFile);
    connect (ui->exitBtn, &QPushButton::clicked, this, &SendFile::endSend);


//    connect (sender, &RdtSender::updateProgress, this, &SendFile::updateProcess);
//    connect (this, &SendFile::startSend, sender, &RdtSender::startSend);
//    connect (sender, &RdtSender::finish, this, &SendFile::finishSend);
//    qDebug() << QThread::currentThreadId ();
//    sender = new RdtSender();
//    connect (sender, &RdtSender::updateProgress, this, &SendFile::updateProcess, Qt::QueuedConnection);
//    connect (this, &SendFile::startSend, sender, &RdtSender::startSend, Qt::QueuedConnection);
//    connect (sender, &RdtSender::finish, this, &SendFile::finishSend, Qt::QueuedConnection);
//    thread.start ();
}


SendFile::~SendFile()
{
//    thread.quit ();
//    thread.wait ();
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
//    sender = new RdtSender(this);
}

void SendFile::updateProcess(qint64 t)
{
//    if (!(t % 24000)) return;
    float useTime = time.elapsed ();
    ui->speedLabel->setText (tr("已发送 %1MB (%2MB/s)").arg (t / (1000 * 1000))
                             .arg ((static_cast<float_t>(t - saveT)/ (useTime - saveTime)) / 1000, 0, 'f', 2));
//    ui->progressBar->setMaximum (file->size ());
    saveT = t;
    saveTime = useTime;
    ui->progressBar->setValue (t);
}

void SendFile::finishSend()
{
    ui->progressBar->setValue (ui->progressBar->maximum ());
    ui->fileNameLabel->setText (tr("发送完毕"));
    ui->speedLabel->clear ();
    ui->exitBtn->setEnabled (true);
    ui->sendBtn->setEnabled (false);
    if (sender) {
//        sender->disconnectFromHost ();
        sender->RdtSender::~RdtSender ();
    }
}

void SendFile::sendFile()
{
    ui->progressBar->setMaximum (file->size ());
    ui->fileNameLabel->setText (tr("正在发送 %1 ...").arg(fileName));

    sender = new RdtSender(file, destination, destinationPort);

    connect (sender, &RdtSender::updateProgress, this, &SendFile::updateProcess);
    connect (this, &SendFile::startSend, sender, &RdtSender::startSend);
    connect (sender, &RdtSender::finish, this, &SendFile::finishSend);
    sender->setFile (file);
    sender->bindListen (address, port);
    emit startSend ();
    time.start ();
//    sender->startSend ();

//    if (sender) {
//        sender->disconnectFromHost ();
//        sender->deleteLater ();
//    }
//    thread = new SendFileThread(this);
//    thread->sender->setFile (file);
//    thread->sender->setDestination (destination, destinationPort);
//    thread->sender->bindListen (address, port);
//    connect (thread, &SendFileThread::updateProcess, this, &SendFile::updateProcess, Qt::QueuedConnection);
//    connect (thread, &SendFileThread::finishSend, this, &SendFile::finishSend, Qt::QueuedConnection);
//    connect (thread, &SendFileThread::finished, thread, &SendFileThread::deleteLater);
//    thread->start ();
//    time.start ();
}

void SendFile::endSend()
{
//    thread->stop ();
//    while(! thread->isFinished ()) {
//        qDebug() << "kill";
//    }
//    thread->deleteLater ();
//    qDebug() << "kill all";
    close ();
}
