#include "receiverequestmanager.h"
#include <QSerialPortInfo>

ReceiveRequestManager::ReceiveRequestManager(QObject *parent):
        QObject(parent),
        m_standardOutput(stdout)
{
    connect(&m_receiveThread, &ReceiveThread::request, this, &ReceiveRequestManager::handleRequest);
    connect(&m_receiveThread, &ReceiveThread::error, this, &ReceiveRequestManager::handleError);
    connect(&m_receiveThread, &ReceiveThread::timeout, this, &ReceiveRequestManager::handleTimeout);

    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
        m_serialPortNameList.append(info.portName());

    m_curentPort = 0;

    m_standardOutput << tr("Status: Running, connected to port %1.")
                           .arg(m_serialPortNameList.at(m_curentPort));
    m_receiveThread.startReceive(m_serialPortNameList.at(m_curentPort),
                        12,
                        "aleyk");
}

void ReceiveRequestManager::handleRequest(const QString &s)
{
    m_standardOutput << QObject::tr("On port %1, request: %2")
                        .arg(m_serialPortNameList.at(m_curentPort))
                        .arg(s)
                     << endl;
}

void ReceiveRequestManager::handleError(const QString &s)
{
    m_standardOutput << QObject::tr("An I/O error occurred on port %1, error: %2")
                        .arg(m_serialPortNameList.at(m_curentPort))
                        .arg(s)
                     << endl;
}

void ReceiveRequestManager::handleTimeout(const QString &s)
{
    m_standardOutput << QObject::tr("Operation timed out for port %1, error: %2")
                        .arg(m_serialPortNameList.at(m_curentPort))
                        .arg(s)
                     << endl;
}
