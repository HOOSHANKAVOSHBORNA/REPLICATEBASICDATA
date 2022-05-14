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
    QSqlRelationalTableModel* getRelationalModelTableName(QString _name);
    int getRequestTypeIndex(QString _typeName) const;
    int getRequestStatusIndex(QString _statusName) const;
    int getTableIndex(QString _tableName) const;
    int getAckStatusIndex(QString _statusName) const;
private:
    DBManager();
    QSqlDatabase db;
    static DBManager *instance;
};

#endif // DBMANAGER_H
