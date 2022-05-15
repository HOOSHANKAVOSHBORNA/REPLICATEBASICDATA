#ifndef RECEIVEREQUESTMANAGER_H
#define RECEIVEREQUESTMANAGER_H

#include <QByteArray>
#include <QObject>
#include <QSerialPort>
#include <QTextStream>
#include <QTimer>

#include "receivethread.h"

class ReceiveRequestManager : public QObject
{
    Q_OBJECT
public:
    ReceiveRequestManager(QObject *parent = nullptr);
private slots:
    void handleRequest(const QString &s);
    void handleError(const QString &s);
    void handleTimeout(const QString &s);
private:
    QTextStream m_standardOutput;
    ReceiveThread m_receiveThread;
    QList<QString> m_serialPortNameList;
    int m_curentPort;
};

#endif // RECEIVEREQUESTMANAGER_H
