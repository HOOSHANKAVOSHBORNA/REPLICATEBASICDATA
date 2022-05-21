#ifndef SERIALPORTMANAGER_H
#define SERIALPORTMANAGER_H

#include <QByteArray>
#include <QObject>
#include <QSerialPort>
#include <QTextStream>
#include <QTimer>

class SerialPortManager : public QObject
{
    Q_OBJECT


public:
    struct PortInfo{
        QString portName;
        QString name;
        int id;
        int index;
    };
    SerialPortManager(PortInfo portInfo, QObject *parent = nullptr);
    void sendData(const QByteArray &data);
    void openPort();
    void closePort();
signals:
    void receiveData(PortInfo, QByteArray &);
private slots:
    void readData();
    void handleError(QSerialPort::SerialPortError);
    void handleRead();
private:
    QTextStream m_standardOutput;
    QSerialPort *m_serial = nullptr;
    QList<QString> m_serialPortNameList;
    PortInfo m_currentPortInfo;
    QByteArray m_readData;
    bool m_isHeaderRead;
};

#endif // SERIALPORTMANAGER_H
