#include "dbmanager.h"
#include <QDebug>
#include <QSqlRecord>
#include <QFile>
#include <QSettings>
#include <QDir>
#include <QSqlQuery>

DBManager* DBManager::instance= nullptr;
DBManager::DBManager()
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
    }

    //db = QSqlDatabase::addDatabase("QPSQL");
    //db->addDatabase("QPSQL");
    db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName(host);
    db.setPort(port);
    db.setDatabaseName(dbName);
    db.setUserName(userName);
    db.setPassword(password);
}

bool DBManager::openConnection()
{
    if(db.isOpen())
        return true;
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
QSqlRelationalTableModel *DBManager::getRelationalModelTableName(QString _name)
{
    QSqlRelationalTableModel* model = new QSqlRelationalTableModel();
    model->setTable(_name);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();
    return model;
}
int DBManager::getAckStatusIndex(QString _statusName) const
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM ack_status WHERE name = '" + _statusName +"'", db);
    if(model->lastError().isValid())
    {
        qDebug() << "getAckStatusIndex -> SQL ERROR: " << model->lastError().text();
    }
    return model->record(0).value("id").toInt();
}


DBManager *DBManager::getDBManager()
{
    if (!instance)
          instance = new DBManager();
    return instance;
}

