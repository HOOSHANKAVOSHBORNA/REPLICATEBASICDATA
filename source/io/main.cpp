#include <QCoreApplication>
#include "serialportmanager.h"
#include <QSerialPortInfo>
#include <QList>
#include "requestmanager.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    RequestManager reqm;
    //reqm.initSerialPorts();

    return a.exec();
}
