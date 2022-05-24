#ifndef PACKETMANAGER_H
#define PACKETMANAGER_H

#include "dbmanager.h"

#include <QByteArray>
#include <QSqlRecord>
#include <QDataStream>
#include <QSqlField>



class PacketManager
{
public:
    PacketManager();
    static QByteArray toByteArray(const QSqlRecord & rec);
    static QSqlRecord fromByteArray(const QByteArray & _data, DBManager *dbm, QString _tableName);
};

#endif // PACKETMANAGER_H
