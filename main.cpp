#include <QApplication>
#include "phonemanager.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    // TODO: Move resources into QT Project
    // chdir("../phoneManager");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

#ifndef NO_FULLSCREEN
    w.showFullScreen();
#endif

    return a.exec();
}
