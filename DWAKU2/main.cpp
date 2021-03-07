#include "mainwindow.hpp"

#include <QApplication>

#include "singleapplication.h"

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    SingleApplication a(argc, argv);

    if(a.isSecondary()) a.exit(0);

    MainWindow w;
    w.show();
    w.OnShow();
    return a.exec();
}
