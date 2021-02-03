#include "mainwindow.hpp"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //if(a.isSecondary()) a.exit(0);

    MainWindow w;
    w.show();
    w.OnShow();
    return a.exec();
}
