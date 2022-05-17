#include <QCoreApplication>
#include "serialportmanager.h"
#include <QSerialPortInfo>
#include <QList>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    //SendRequestManager sendRequest;
    //ReceiveRequestManager receiveRequest;
    QList<SerialPortManager*> portManagerList;
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
    {
        if(info.portName().contains("USB"))
        {
            SerialPortManager *serialPortManager = new SerialPortManager(info.portName());
            serialPortManager->closePort();
            serialPortManager->openPort();
            portManagerList.append(serialPortManager);
        }
    }

//    SerialPortManager serialPortManager0(0);
//    serialPortManager0.openPort();
//    serialPortManager0.writeData(QString("salam").toUtf8());

//    SerialPortManager serialPortManager1(1);
//    serialPortManager1.openPort();
//    serialPortManager1.writeData(QString("salam").toUtf8());

    return a.exec();
}
