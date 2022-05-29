#include "requestmanager.h"

#include <QDataStream>
#include <QSerialPortInfo>
#include <QSqlField>
#include <QSqlRelationalTableModel>
#include "../common/packetmanager.h"


RequestManager::RequestManager(QObject *parent):
    QObject(parent),
    m_standardOutput(stdout)
{
    m_dbm  = DBManager::getDBManager();
    //m_dbm->openConnection();

    //init and open port
    initSerialPorts();
    connect(&m_timer, &QTimer::timeout, this, &RequestManager::checkAcknowledgment);
    m_timer.start(7000);

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
    QByteArray data = PacketManager::toByteArray(rec);
    // add header----------------
    QByteArray result;
    QDataStream stream(&result,QIODevice::ReadWrite);
    stream.setVersion(QDataStream::Qt_5_13);
    qint8 type = DataType::Request;//request
    //data.prepend(type);
    qint32 len = static_cast<qint32>(data.length()) + static_cast<qint32>(sizeof(type));
    stream << len ;//data len + type len(int8 = 1)
    stream << type;
    //stream << data;
    data.prepend(result);
    //-------------------------------
    m_serialPortMap[receiver]->sendData(data);
}

void RequestManager::sendAcknowledgment(int receiver, qint64 reqId)
{
    if(!m_serialPortMap.contains(receiver))
    {
        m_standardOutput <<QObject::tr("Can not find connection to: %1.")
                           .arg(receiver)
                        << endl;
        return;
    }
    QByteArray result;
    QDataStream stream(&result,QIODevice::ReadWrite);
    stream.setVersion(QDataStream::Qt_5_13);
    qint8 type = DataType::Ack;
    //data.prepend(type);
    qint32 len = sizeof(reqId) + sizeof(type);
    stream << len; //type len(int8 = 1)
    stream << type;
    stream << reqId;
    //-------------------------------
    m_serialPortMap[receiver]->sendData(result);
}

void RequestManager::onReceiveData(SerialPortManager::PortInfo info, QByteArray &data)
{
    QDataStream dataStream(data);
    qint8 type;
    qint32 len;
    dataStream >> len >> type;

    data.remove(0,5);
    if(type == DataType::Request)
    {
        QSqlRecord rec = PacketManager::fromByteArray(data, m_dbm, "request");
        QSqlRelationalTableModel *model = m_dbm->getRelationalModelTableName("request");
        qint64 reqId = rec.value("id").toLongLong();
        rec.remove(rec.indexOf("id"));
        qint64 tableId = rec.value("table_id").toLongLong();
        qint64 createdAt = rec.value("created_at").toLongLong();
        //already exist----------------------------------------------
        model->setFilter(tr("table_id = %1 AND created_at = %2").arg(tableId).arg(createdAt));
        model->select();
        if(model->rowCount() > 0)
        {
            QSqlRecord existRec = model->record(0);
            int status = rec.value(6).toInt();
            existRec.setValue(6, status);
            model->setRecord(0, existRec);
            if(!model->submitAll())
            {
                m_standardOutput << tr("Can not submit request '%1' that received from '%2' -> SQL ERROR:%3 ")
                                 .arg(reqId)
                                 .arg(info.name)
                                 .arg( model->lastError().text())
                                 << endl;
                model->revertAll();
                return;
            }
            sendAcknowledgment(info.id, reqId);
            return;
        }
        //----------------------------------------------
        if(!model->insertRecord(-1, rec))
        {
            m_standardOutput << tr("Can not insert request '%1' that received from '%2'")
                             .arg(reqId)
                             .arg(info.name)
                             << endl;
            return;
        }
        if(!model->submitAll())
        {
            m_standardOutput << tr("Can not submit request '%1' that received from '%2' -> SQL ERROR:%3 ")
                             .arg(reqId)
                             .arg(info.name)
                             .arg( model->lastError().text())
                             << endl;
            model->revertAll();
            return;
        }
        //send ack
        sendAcknowledgment(info.id, reqId);
    }
    else if(type == DataType::Ack)
    {
        QDataStream stream(data);
        stream.setVersion(QDataStream::Qt_5_13);
        quint64 reqId;
        stream >> reqId;
        QSqlRelationalTableModel *model = m_dbm->getRelationalModelTableName("acknowledgment");
        int sendedIndex = m_dbm->getAckStatusIndex("sended");
        model->setFilter(tr("request_id = %1 AND receiver = %2").arg(reqId).arg(info.id));
        model->select();
        if(model->rowCount() < 1)
        {
            m_standardOutput <<QObject::tr("Can not find request '%1' acknowledgment for receiver '%2'.")
                               .arg(reqId)
                               .arg(info.id)
                            << endl;
            return;
        }
        QSqlRecord rec = model->record(0);
        rec.setValue("status", sendedIndex);
        model->setRecord(0, rec);
        if(!model->submitAll())
        {
            m_standardOutput << tr("Can not submit acknowledgment whit request id '%1' that received from '%2' -> SQL ERROR:%3 ")
                             .arg(reqId)
                             .arg(info.name)
                             .arg( model->lastError().text())
                             << endl;
        }
    }
    else
    {
        m_standardOutput << tr("Received data is wrong: '%1' ")
                         .arg(QString(data.toHex()))
                         << endl;
    }

}
