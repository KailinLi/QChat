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
