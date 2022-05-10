#ifndef DBMANAGER_H
#define DBMANAGER_H

//#include <QtSql/QPSQLDriver>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QSqlRelationalTableModel>
#include "datastrucs.h"

class DBManager
{
public:
    static DBManager *getDBManager();
    bool openConnection();
    void closeConnection();
    QList<Request>* loadRequests();
    QSqlQueryModel* getRequestModel();
    QSqlRelationalTableModel* getRequestRelationalModel();
    QSqlQueryModel* getTableNameModel();
private:
    DBManager();
    QSqlDatabase db;
    static DBManager *instance;
};

#endif // DBMANAGER_H
