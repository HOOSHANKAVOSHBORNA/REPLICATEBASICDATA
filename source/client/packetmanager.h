#ifndef PACKETMANAGER_H
#define PACKETMANAGER_H

#include "dbmanager.h"

#include <QByteArray>
#include <QSqlRecord>



class PacketManager
{
public:
    PacketManager();
    static QByteArray toByteArray(const QSqlRecord &);
    static QSqlRecord fromByteArray(const QByteArray &, DBManager *dbm, QString);
};

#endif // PACKETMANAGER_H
