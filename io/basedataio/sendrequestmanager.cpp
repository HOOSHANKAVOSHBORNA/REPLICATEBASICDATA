#include <QSerialPortInfo>
#include "sendrequestmanager.h"

SendRequestManager::SendRequestManager(QObject *parent):
        QObject(parent),
        m_standardOutput(stdout)
{
    connect(&m_sendThread, &SendThread::response, this, &SendRequestManager::handleResponse);
    connect(&m_sendThread, &SendThread::error, this, &SendRequestManager::handleError);
    connect(&m_sendThread, &SendThread::timeout, this, &SendRequestManager::handleTimeout);

    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
        m_serialPortNameList.append(info.portName());

    m_curentPort = 0;

    m_standardOutput <<tr("Status: Running, connected to port %1.")
                           .arg(m_serialPortNameList.at(m_curentPort))
                    << endl;
    m_sendThread.transaction(m_serialPortNameList.at(m_curentPort),
                         15,
                         "salam");
}

void SendRequestManager::handleResponse(const QString & _response)
{
    m_standardOutput << QObject::tr("On port %1, respons: %2")
                        .arg(m_serialPortNameList.at(m_curentPort))
                        .arg(_response)
                     << endl;
}
void SendRequestManager::handleError(const QString & _message)
{
    m_standardOutput << QObject::tr("An I/O error occurred on port %1, error: %2")
                        .arg(m_serialPortNameList.at(m_curentPort))
                        .arg(_message)
                     << endl;
}

void SendRequestManager::handleTimeout(const QString & _message)
{
    m_standardOutput << QObject::tr("Operation timed out for port %1, error: %2")
                        .arg(m_serialPortNameList.at(m_curentPort))
                        .arg(_message)
                     << endl;
}


