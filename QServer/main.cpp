#include "qserver.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QServer w;
    w.show();

    return a.exec();
}
