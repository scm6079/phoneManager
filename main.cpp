#include <QCoreApplication>
#include "phonemanager.h"
#include <unistd.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    chdir("../phoneManager");

    PhoneManager phoneManager;


    return a.exec();
}
