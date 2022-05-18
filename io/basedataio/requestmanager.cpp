#include "requestmanager.h"

#include <QDataStream>
#include <QSerialPortInfo>
#include <QSqlField>
#include <QSqlRelationalTableModel>


RequestManager::RequestManager(QObject *parent):
    QObject(parent),
    m_standardOutput(stdout)
{
    m_dbm  = DBManager::getDBManager();
    m_dbm->openConnection();

    //init and open port
    initSerialPorts();
    connect(&m_timer, &QTimer::timeout, this, &RequestManager::checkAcknowledgment);
    m_timer.start(5000);

}

void RequestManager::initSerialPorts()
{
    const QList<QSerialPortInfo> infos = QSerialPortInfo::availablePorts();
    QSqlRelationalTableModel *model = m_dbm->getRelationalModelTableName("client_info");
    for(int i = 0; i < model->rowCount(); i++)
    {
        SerialPortManager::PortInfo portManagerInfo;
        QSqlRecord rec = model->record(i);
        portManagerInfo.id = rec.value("id").toInt();
        portManagerInfo.name = rec.value("name").toString();
        portManagerInfo.index = rec.value("port_index").toInt();
        if(portManagerInfo.index == -1)
            continue;//it is my info
        if(portManagerInfo.index >= infos.length())
        {
            m_standardOutput <<QObject::tr("Can not find connection for port %1."
                                           " pleas check connection and restart program.")
                               .arg(portManagerInfo.name)
                            << endl;
        }
        else
        {
            portManagerInfo.portName = infos.at(portManagerInfo.index).portName();
            SerialPortManager *serialPortManager = new SerialPortManager(portManagerInfo, this);
            connect(serialPortManager, &SerialPortManager::receiveData, this, &RequestManager::onReceiveData);
            serialPortManager->closePort();
            serialPortManager->openPort();
            m_serialPortMap[portManagerInfo.id] = serialPortManager;
        }

    }
}

void RequestManager::checkAcknowledgment()
{
    QSqlRelationalTableModel *model = m_dbm->getRelationalModelTableName("acknowledgment");
    int pendingIndex = m_dbm->getAckStatusIndex("pending");
    model->setFilter(tr("status = %1").arg(pendingIndex));
    model->select();
    for(int i = 0; i < model->rowCount(); i++)
    {
        QSqlRecord rec = model->record(i);
        int request_id = rec.value("request_id").toInt();
        int receiver = rec.value("receiver").toInt();
        sendRequest(request_id, receiver);
    }
}

void RequestManager::sendRequest(int request_id, int receiver)
{
    QSqlRelationalTableModel *model = m_dbm->getRelationalModelTableName("request");
    model->setFilter(tr("id = %1").arg(request_id));
    model->select();
    if(model->rowCount() < 1)
    {
        m_standardOutput <<QObject::tr("Can not find request: %1.")
                           .arg(request_id)
                        << endl;
        return;
    }
    if(!m_serialPortMap.contains(receiver))
    {
        m_standardOutput <<QObject::tr("Can not find connection to: %1.")
                           .arg(receiver)
                        << endl;
        return;
    }
    QSqlRecord rec = model->record(0);
    QByteArray data = toByteArray(rec);
    // add header----------------
    QByteArray result;
    QDataStream stream(&result,QIODevice::ReadWrite);
    stream.setVersion(QDataStream::Qt_5_13);
    qint8 type = 1;//request
    //data.prepend(type);
    qint32 len = static_cast<qint32>(data.length());
    stream << (len + 1);//data len + type len(int8 = 1)
    stream << type;
    stream << data;
    //-------------------------------
    m_serialPortMap[receiver]->sendData(result);
}

QByteArray RequestManager::toByteArray(QSqlRecord _rec)
{
    QByteArray result;
    QDataStream stream(&result,QIODevice::ReadWrite);
    stream.setVersion(QDataStream::Qt_5_13);
    for (int i = 0; i < _rec.count(); i++)
    {
        QSqlField field = _rec.field(i);
        stream << field.value();
    }
    return result;
}

QSqlRecord RequestManager::fromByteArray(QByteArray _data, QString _tableName)
{
    QDataStream stream(_data);
    stream.setVersion(QDataStream::Qt_5_13);
    QSqlRelationalTableModel *model = m_dbm->getRelationalModelTableName(_tableName);
    QSqlRecord rec = model->record();
    for (int i = 0; i < rec.count(); i++)
    {
        QVariant field;
        stream >> field;
        rec.setValue(i,field);
    }
    return rec;
}

void RequestManager::onReceiveData(SerialPortManager::PortInfo info, const QByteArray &data)
{

}
