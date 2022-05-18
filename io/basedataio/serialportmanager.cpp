#include <QSerialPortInfo>
#include "serialportmanager.h"

SerialPortManager::SerialPortManager(PortInfo portInfo, QObject *parent):
    QObject(parent),
    m_standardOutput(stdout),
    m_serial(new QSerialPort(this))
{
    connect(m_serial, &QSerialPort::errorOccurred, this, &SerialPortManager::handleError);
    connect(m_serial, &QSerialPort::readyRead, this, &SerialPortManager::readData);

    m_currentPortInfo = portInfo;

    m_standardOutput <<QObject::tr("Status: Running, connected to port %1: %2")
                       .arg(m_currentPortInfo.portName)
                       .arg(m_currentPortInfo.name)
                    << endl;

    m_isHeaderRead = false;
}

void SerialPortManager::sendData(const QByteArray &data)
{
    m_serial->write(data);
    const qint64 bytesWritten = m_serial->write(data);
    const QString request = QString::fromUtf8(data);
    m_standardOutput <<tr("Write data: '%1' to port %2: %3.")
                       .arg(request)
                       .arg(m_serial->portName())
                       .arg(m_currentPortInfo.name)
                    << endl;

    if (bytesWritten == -1) {
        m_standardOutput << QObject::tr("Failed to write the data to port %1: %2, error: %3")
                            .arg(m_serial->portName())
                            .arg(m_currentPortInfo.name)
                            .arg(m_serial->errorString())
                         << endl;
//        QCoreApplication::exit(1);
    } else if (bytesWritten != data.size()) {
        m_standardOutput << QObject::tr("Failed to write all the data to port %1: %2, error: %3")
                            .arg(m_serial->portName())
                            .arg(m_currentPortInfo.name)
                            .arg(m_serial->errorString())
                         << endl;
//        QCoreApplication::exit(1);
    }
}

void SerialPortManager::readData()
{

    //const QByteArray data = m_serial->readAll();
    //QDataStream stream(_data);
    //stream.setVersion(QDataStream::Qt_5_13);
    if (m_serial->bytesAvailable() < 4)
    {
        return;
    }
    qint32 len;
    if(!m_isHeaderRead)
    {
        const QByteArray data = m_serial->read(4);
        len = data.toInt();
        m_isHeaderRead = true;
        m_readData.append(data);
    }

    if (m_serial->bytesAvailable() < len)
    {
        return;
    }
    const QByteArray data = m_serial->read(len);
    m_readData.append(data);
    emit receiveData(m_currentPortInfo, m_readData);
    handleRead();
    m_isHeaderRead = false;

}

void SerialPortManager::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::WriteError)
    {
        m_standardOutput << tr("An I/O error occurred while writing"
                                        " the data to port %1: %2, error: %3")
                            .arg(m_serial->portName())
                            .arg(m_currentPortInfo.name)
                            .arg(m_serial->errorString())
                         << endl;
    }
    if (error == QSerialPort::ReadError) {
        m_standardOutput << QObject::tr("An I/O error occurred while reading "
                                        "the data from port %1: %2, error: %3")
                            .arg(m_serial->portName())
                            .arg(m_currentPortInfo.name)
                            .arg(m_serial->errorString())
                         << endl;
    }
}


void SerialPortManager::handleRead()
{
    if (m_readData.isEmpty()) {
//        m_standardOutput << QObject::tr("No data was currently available "
//                                        "for reading from port %1")
//                            .arg(m_serial->portName())
//                         << endl;
    } else {
        m_standardOutput <<tr("Read data: '%1' from port %2: %3.")
                               .arg(QString::fromUtf8(m_readData))
                               .arg(m_serial->portName())
                           .arg(m_currentPortInfo.name)
                            << endl;
    }
    //-----------------------------------------
    m_readData.clear();
}

void SerialPortManager::openPort()
{
    m_serial->setPortName(m_currentPortInfo.portName);
    //m_serial->setBaudRate(p.baudRate);
    //m_serial->setDataBits(p.dataBits);
    //m_serial->setParity(p.parity);
    //m_serial->setStopBits(p.stopBits);
    //m_serial->setFlowControl(p.flowControl);
    if (!m_serial->open(QIODevice::ReadWrite))
    {
        m_standardOutput << tr("Can't open %1: %2, error code %3")
                            .arg(m_serial->portName())
                            .arg(m_currentPortInfo.name)
                            .arg(m_serial->errorString());
    }
}

void SerialPortManager::closePort()
{
    if (m_serial->isOpen())
        m_serial->close();
}
