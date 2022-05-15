#include <QCoreApplication>
#include "sendrequestmanager.h"
#include "receiverequestmanager.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    //SendRequestManager sendRequest;
    ReceiveRequestManager receiveRequest;
    return a.exec();
}
