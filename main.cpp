#include "mypro.h"
#include <QPushButton>
#include <QApplication>
#include <QAction>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MyPro w;
    w.resize(800,600);
    w.show();
    return QApplication::exec();
}
