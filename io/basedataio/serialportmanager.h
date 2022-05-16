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
    SerialPortManager(QString portNumber, QObject *parent = nullptr);
    void writeData(const QByteArray &data);
    void openPort();
    void closePort();
private slots:
    void readData();
    void handleError(QSerialPort::SerialPortError);
    void handleTimeout();
private:
private:
    QTextStream m_standardOutput;
    QSerialPort *m_serial = nullptr;
    QList<QString> m_serialPortNameList;
    QString m_curentPort;
    QTimer m_timer;
};

#endif // SERIALPORTMANAGER_H
