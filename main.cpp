#include <QCoreApplication>
#include "phonemanager.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    PhoneManager phoneManager;

    return a.exec();
}
