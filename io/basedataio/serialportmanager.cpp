#include <QSerialPortInfo>
#include "serialportmanager.h"

SerialPortManager::SerialPortManager(QString portName, QObject *parent):
    QObject(parent),
    m_standardOutput(stdout),
    m_serial(new QSerialPort(this))
{
    connect(m_serial, &QSerialPort::errorOccurred, this, &SerialPortManager::handleError);
    connect(m_serial, &QSerialPort::readyRead, this, &SerialPortManager::readData);

    m_curentPort = portName;

    m_standardOutput <<QObject::tr("Status: Running, connected to port %1.")
                           .arg(m_curentPort)
                    << endl;

    connect(&m_timer, &QTimer::timeout, this, &SerialPortManager::handleTimeout);

    m_timer.start(5000);
    m_isHeaderRead = false;
}

void SerialPortManager::writeData(const QByteArray &data)
{
    m_serial->write(data);
    const qint64 bytesWritten = m_serial->write(data);
    const QString request = QString::fromUtf8(data);
    m_standardOutput <<tr("Write data: '%1' to port %2.")
                       .arg(request)
                       .arg(m_serial->portName())
                    << endl;

    if (bytesWritten == -1) {
        m_standardOutput << QObject::tr("Failed to write the data to port %1, error: %2")
                            .arg(m_serial->portName())
                            .arg(m_serial->errorString())
                         << endl;
//        QCoreApplication::exit(1);
    } else if (bytesWritten != data.size()) {
        m_standardOutput << QObject::tr("Failed to write all the data to port %1, error: %2")
                            .arg(m_serial->portName())
                            .arg(m_serial->errorString())
                         << endl;
//        QCoreApplication::exit(1);
    }
}

void SerialPortManager::readData()
{

    //const QByteArray data = m_serial->readAll();
    qint8 len;
    if(!m_isHeaderRead)
    {
        const QByteArray data = m_serial->read(1);
        len = data.at(0);
        m_isHeaderRead = true;
    }

    if (m_serial->bytesAvailable() < len)
    {
        return;
    }
    const QByteArray data = m_serial->read(len);
    m_readData.append(data);
    handleRead();
    m_isHeaderRead = false;

}

void SerialPortManager::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::WriteError)
    {
        m_standardOutput << tr("An I/O error occurred while writing"
                                        " the data to port %1, error: %2")
                            .arg(m_serial->portName())
                            .arg(m_serial->errorString())
                         << endl;
    }
    if (error == QSerialPort::ReadError) {
        m_standardOutput << QObject::tr("An I/O error occurred while reading "
                                        "the data from port %1, error: %2")
                            .arg(m_serial->portName())
                            .arg(m_serial->errorString())
                         << endl;
    }
}

void SerialPortManager::handleTimeout()
{
    QByteArray data = QString("salam#").toUtf8();
    qint8 len = (qint8)data.length();
    data.prepend(len);
    writeData(data);
    //m_timer.start(5000);
}

void SerialPortManager::handleRead()
{
    if (m_readData.isEmpty()) {
//        m_standardOutput << QObject::tr("No data was currently available "
//                                        "for reading from port %1")
//                            .arg(m_serial->portName())
//                         << endl;
    } else {
        m_standardOutput <<tr("Read data: '%1' from port %2.")
                               .arg(QString::fromUtf8(m_readData))
                               .arg(m_serial->portName())
                            << endl;
    }
    //-----------------------------------------
    m_readData.clear();
}

void SerialPortManager::openPort()
{
    m_serial->setPortName(m_curentPort);
    //m_serial->setBaudRate(p.baudRate);
    //m_serial->setDataBits(p.dataBits);
    //m_serial->setParity(p.parity);
    //m_serial->setStopBits(p.stopBits);
    //m_serial->setFlowControl(p.flowControl);
    if (!m_serial->open(QIODevice::ReadWrite))
    {
        m_standardOutput << tr("Can't open %1, error code %2")
                              .arg(m_serial->portName()).arg(m_serial->errorString());
    }
}

void SerialPortManager::closePort()
{
    if (m_serial->isOpen())
        m_serial->close();
}
