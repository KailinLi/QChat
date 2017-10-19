#ifndef QCLIENT_H
#define QCLIENT_H

#include <QWidget>

namespace Ui {
class QClient;
}

class QClient : public QWidget
{
    Q_OBJECT

public:
    explicit QClient(QWidget *parent = 0);
    ~QClient();

private:
    Ui::QClient *ui;
};

#endif // QCLIENT_H
