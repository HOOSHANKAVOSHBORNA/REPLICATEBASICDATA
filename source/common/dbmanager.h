#ifndef DBMANAGER_H
#define DBMANAGER_H

//#include <QtSql/QPSQLDriver>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QSqlRelationalTableModel>
#include <QSettings>
#include <QFile>
#include <QDebug>
#include <QSqlRecord>

class DBManager
{
public:
    static DBManager *getDBManager();
    bool openConnection();
    void closeConnection();
    QSqlQueryModel* getRequestModel();
    QSqlRelationalTableModel* getRequestRelationalModel();
    QSqlQueryModel* getTableNameModel();
    QSqlRelationalTableModel* getRelationalModelTableName(QString _name);
    int getRequestTypeIndex(QString _typeName) const;
    int getRequestStatusIndex(QString _statusName) const;
    int getTableIndex(QString _tableName) const;
    int getAckStatusIndex(QString _statusName) const;
    int getSelfId() const;
    int getReviewerId() const;
    bool isReviewer() const;
    bool isSended(int reqId, int reciver) const;
    bool isSended(int reqId) const;
    bool isApplicant(int reqId) const;
    bool hasApplied(int reqId) const;
private:
    DBManager();
    QSqlDatabase db;
    static DBManager *instance;
};
#endif // DBMANAGER_H
