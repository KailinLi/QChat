#ifndef QSERVER_H
#define QSERVER_H

#include <QWidget>

namespace Ui {
class QServer;
}

class QServer : public QWidget
{
    Q_OBJECT

public:
    explicit QServer(QWidget *parent = 0);
    ~QServer();

private:
    Ui::QServer *ui;
};

#endif // QSERVER_H
