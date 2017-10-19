#include "qclient.h"
#include "ui_qclient.h"

QClient::QClient(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QClient)
{
    ui->setupUi(this);
}

QClient::~QClient()
{
    delete ui;
}
