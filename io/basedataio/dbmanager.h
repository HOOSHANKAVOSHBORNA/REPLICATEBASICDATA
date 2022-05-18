#ifndef DBMANAGER_H
#define DBMANAGER_H


#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QSqlRelationalTableModel>
//#include "datastrucs.h"

class DBManager
{
public:
    static DBManager *getDBManager();
    bool openConnection();
    void closeConnection();
    QSqlRelationalTableModel* getRelationalModelTableName(QString _name);
    int getAckStatusIndex(QString _statusName) const;
private:
    DBManager();
    QSqlDatabase db;
    static DBManager *instance;
};

#endif // DBMANAGER_H
