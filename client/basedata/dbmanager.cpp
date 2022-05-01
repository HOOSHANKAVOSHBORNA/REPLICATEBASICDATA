#include "dbmanager.h"
#include <QDebug>
#include <QSqlRecord>

DBManager::DBManager()
{

}

bool DBManager::openConnection()
{
    db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("127.0.0.1");
    db.setPort(5432);
    db.setDatabaseName("basedata");
    db.setUserName("postgres");
    db.setPassword("qazwsx");
    bool ok = db.open();
    if(!ok)
    {
        qDebug() << db.lastError().text();
    }
    else
    {
        qDebug() << "db is connected.";
    }
    return ok;
}

void DBManager::closeConnection()
{
    db.close();
}

QList<request> *DBManager::loadRequests()
{
    QList<request>* result = new QList<request>();
    QSqlQuery query(db);
    QString sql = "SELECT * FROM request;";
    if (query.exec(sql) != false) {
        while (query.next())
        {
            request req;
            QSqlRecord rec = query.record();
            req.id = query.value(rec.indexOf("id")).toLongLong();
            req.table_id = query.value(rec.indexOf("table_id")).toLongLong();
            req.table_name_id = query.value(rec.indexOf("table_name_id")).toInt();
            req.applicant = query.value(rec.indexOf("applicant")).toInt();
            req.reviewer = query.value(rec.indexOf("reviewer")).toInt();
            req.type = (int16_t)query.value(rec.indexOf("type")).toInt();
            req.status = (int16_t)query.value(rec.indexOf("status")).toInt();
            req.data = query.value(rec.indexOf("data")).toByteArray();
            req.description = query.value(rec.indexOf("description")).toString();
            req.created_at = query.value(rec.indexOf("created_at")).toLongLong();

            result->append(req);
        }
    }
    else
        qDebug() << "SQL ERROR: " << query.lastError().text();
    return result;
}
