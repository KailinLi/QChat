#include "qserver.h"
#include "ui_qserver.h"

QServer::QServer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QServer)
{
    ui->setupUi(this);
}

QServer::~QServer()
{
    delete ui;
}
