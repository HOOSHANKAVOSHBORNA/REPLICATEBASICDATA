#include "dbmanager.h"
#include <QDebug>
#include <QSqlRecord>
#include <QFile>
#include <QSettings>
#include <QDir>

DBManager::DBManager()
{

}

bool DBManager::openConnection()
{
    QString host,dbName, userName, password;
    int port;

    if(QFile("dbconfig.ini").exists())
    {
        QSettings settings("dbconfig.ini",QSettings::IniFormat);
        settings.sync();
        settings.beginGroup("database");
        host = settings.value("host").toString();
        port = settings.value("port").toInt();
        dbName = settings.value("dbName").toString();
        userName = settings.value("userName").toString();
        password = settings.value("password").toString();
        settings.endGroup();
    }
    else
    {
        qDebug() << "dbconfig.ini not exist.";
        return false;
    }

    db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName(host);
    db.setPort(port);
    db.setDatabaseName(dbName);
    db.setUserName(userName);
    db.setPassword(password);
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

QSqlQueryModel *DBManager::getRequestModel()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM request ORDER BY id DESC;",db);
    if(model->lastError().isValid())
    {
        qDebug() << "getRequestModel -> SQL ERROR: " << model->lastError().text();
    }
    return model;
}
