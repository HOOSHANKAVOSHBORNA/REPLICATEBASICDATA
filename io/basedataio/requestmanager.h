#ifndef REQUESTMANAGER_H
#define REQUESTMANAGER_H

#include "dbmanager.h"
#include "serialportmanager.h"

#include <QObject>
#include <QSqlRecord>

class RequestManager : public QObject
{
    Q_OBJECT
public:
    enum DataType{
      Request = 1,
      Ack = 2
    };
    RequestManager(QObject *parent = nullptr);
private:
    void initSerialPorts();
    void sendRequest(int request_id, int receiver);
    void sendAcknowledgment(int receiver, qint64 reqId);
    QByteArray toByteArray(QSqlRecord);
    QSqlRecord fromByteArray(QByteArray, QString);
private slots:
    void checkAcknowledgment();
    void onReceiveData(SerialPortManager::PortInfo, QByteArray &);
private:
    DBManager *m_dbm;
    QTextStream m_standardOutput;
    QTimer m_timer;
    QMap<int, SerialPortManager*> m_serialPortMap;
};

#endif // REQUESTMANAGER_H
