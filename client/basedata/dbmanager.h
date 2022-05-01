#ifndef DBMANAGER_H
#define DBMANAGER_H

//#include <QtSql/QPSQLDriver>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include "datastrucs.h"

class DBManager
{
public:
    DBManager();
    bool openConnection();
    void closeConnection();
    QList<struct request>* loadRequests();
private:
    QSqlDatabase db;
};

#endif // DBMANAGER_H
