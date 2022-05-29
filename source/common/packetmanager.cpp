#include "packetmanager.h"

#include <QDataStream>
#include <QSqlField>

PacketManager::PacketManager()
{

}

QByteArray PacketManager::toByteArray(const QSqlRecord& rec)
{
    QByteArray result;
    QDataStream stream(&result,QIODevice::ReadWrite);
    stream.setVersion(QDataStream::Qt_5_13);
    for (int i = 0; i < rec.count(); i++)
    {
        QSqlField field = rec.field(i);
        stream << field.value();
    }
    return result;
}

QSqlRecord PacketManager::fromByteArray(const QByteArray& _data,DBManager* dbm, QString _tableName)
{
    QDataStream stream(_data);
    stream.setVersion(QDataStream::Qt_5_13);
    QSqlRelationalTableModel *model = dbm->getRelationalModelTableName(_tableName);
    QSqlRecord rec = model->record();
    for (int i = 0; i < rec.count(); i++)
    {
        QVariant field;
        stream >> field;
        rec.setValue(i,field);
    }
    return rec;
}

QByteArray PacketManager::toByteArray(const QByteArray &oldData, const QByteArray &newData)
{
    QByteArray result;
    QDataStream stream(&result,QIODevice::ReadWrite);
    stream.setVersion(QDataStream::Qt_5_13);
    stream << oldData;
    stream << newData;
    return result;
}

QList<QByteArray> PacketManager::fromByteArray(const QByteArray &_data)
{
    QList<QByteArray> result;
    QByteArray oldData;
    QByteArray newData;
    QDataStream stream(_data);
    stream.setVersion(QDataStream::Qt_5_13);
    stream >> oldData;
    stream >> newData;
    result.append(oldData);
    result.append(newData);
    return result;
}
