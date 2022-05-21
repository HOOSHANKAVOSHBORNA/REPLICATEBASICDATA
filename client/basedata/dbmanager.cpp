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

QList<Request> *DBManager::loadRequests()
{
    QList<Request>* result = new QList<Request>();
    QSqlQuery query(db);
    QString sql = "SELECT * FROM request;";
    if (query.exec(sql) != false) {
        while (query.next())
        {
            Request req;
            QSqlRecord rec = query.record();
            req.id = query.value(rec.indexOf("id")).toLongLong();
            req.table_id = query.value(rec.indexOf("table_id")).toLongLong();
            req.table_name = query.value(rec.indexOf("table_name")).toInt();
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

QSqlRelationalTableModel *DBManager::getRequestRelationalModel()
{
    QSqlRelationalTableModel* model = new QSqlRelationalTableModel();
    model->setTable("request");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);

//    QSqlQuery query(db);
//    QString sql = "SELECT " \
//            "tc.table_schema, "\
//            "tc.constraint_name, "\
//            "tc.table_name, "\
//            "kcu.column_name, "\
//            "col.ordinal_position, "\
//            "ccu.table_schema AS foreign_table_schema," \
//            "ccu.table_name AS foreign_table_name," \
//            "ccu.column_name AS foreign_column_name " \
//        "FROM " \
//            "information_schema.table_constraints AS tc "\
//            "JOIN information_schema.key_column_usage AS kcu "\
//              "ON tc.constraint_name = kcu.constraint_name"\
//             " AND tc.table_schema = kcu.table_schema "\
//            "JOIN information_schema.columns as col "\
//                "ON col.column_name = kcu.column_name "\
//                 "AND tc.table_schema = col.table_schema "\
//            "JOIN information_schema.constraint_column_usage AS ccu "\
//              "ON ccu.constraint_name = tc.constraint_name "\
//              "AND ccu.table_schema = tc.table_schema "\
//        "WHERE tc.constraint_type = 'FOREIGN KEY' AND tc.table_name='request';";
//    if (query.exec(sql) != false) {
//        while (query.next())
//        {
//            Request req;
//            QSqlRecord rec = query.record();
//            QString column_name = query.value(rec.indexOf("column_name")).toString();
//            QString foreign_table_name = query.value(rec.indexOf("foreign_table_name")).toString();
//            QString foreign_column_name = query.value(rec.indexOf("foreign_column_name")).toString();
//            int column_index = query.value(rec.indexOf("ordinal_position")).toInt();

//            model->setRelation(column_index - 1, QSqlRelation(foreign_table_name, foreign_column_name, "name"));
//            model->setHeaderData(column_index- 1, Qt::Horizontal, column_name);
//        }
//    }
//    else
//        qDebug() << "SQL ERROR: " << query.lastError().text();

//    QSqlRelation rl("table_name", "id", "name");
//    rl.displayColumn();
    model->setRelation(2, QSqlRelation("table_name", "id", "name"));
    model->setHeaderData(2, Qt::Horizontal, "table_name");

    model->setRelation(3, QSqlRelation("client_info", "id", "name"));
    model->setHeaderData(3, Qt::Horizontal, "applicant");

    model->setRelation(4, QSqlRelation("client_info", "id", "name"));
    model->setHeaderData(4, Qt::Horizontal, "reviewer");

    model->setRelation(5, QSqlRelation("request_type", "id", "name"));
    model->setHeaderData(5, Qt::Horizontal, "type");

    model->setRelation(6, QSqlRelation("request_status", "id", "name"));
    model->setHeaderData(6, Qt::Horizontal, "status");

    model->setSort(0, Qt::DescendingOrder);
    model->select();
    if(model->lastError().isValid())
    {
        qDebug() << "getRequestRelationalModel -> SQL ERROR: " << model->lastError().text();
    }
    return model;
}

QSqlQueryModel *DBManager::getTableNameModel()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT name FROM table_name;",db);
    if(model->lastError().isValid())
    {
        qDebug() << "getTableNameModel -> SQL ERROR: " << model->lastError().text();
    }
    return model;
}

QSqlRelationalTableModel *DBManager::getRelationalModelTableName(QString _name)
{
    QSqlRelationalTableModel* model = new QSqlRelationalTableModel();
    model->setTable(_name);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();
    return model;
}

int DBManager::getRequestTypeIndex(QString _typeName) const
{
    QSqlQueryModel* model = new QSqlQueryModel();
//    QSqlQuery query(db);
//    query.prepare("SELECT * FROM request_type WHERE name = ?;");
//    query.addBindValue(_typeName);
    model->setQuery("SELECT * FROM request_type WHERE name = '" + _typeName +"'", db);
    if(model->lastError().isValid())
    {
        qDebug() << "getRequestTypeIndex -> SQL ERROR: " << model->lastError().text();
    }
    //qDebug()<< model->record(0);
    return model->record(0).value("id").toInt();
}

int DBManager::getRequestStatusIndex(QString _statusName) const
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM request_status WHERE name = '" + _statusName +"'", db);
    if(model->lastError().isValid())
    {
        qDebug() << "getRequestStatusIndex -> SQL ERROR: " << model->lastError().text();
    }
    return model->record(0).value("id").toInt();
}

int DBManager::getTableIndex(QString _tableName) const
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM table_name WHERE name = '" + _tableName +"'", db);
    if(model->lastError().isValid())
    {
        qDebug() << "getTableIndex -> SQL ERROR: " << model->lastError().text();
    }
    return model->record(0).value("id").toInt();
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

int DBManager::getSelfId() const
{
    int portIndex = -1;
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery(QObject::tr("SELECT * FROM client_info WHERE port_index = %1").arg(portIndex), db);
    if(model->lastError().isValid())
    {
        qDebug() << "getAckStatusIndex -> SQL ERROR: " << model->lastError().text();
    }
    return model->record(0).value("id").toInt();
}

int DBManager::getReviewerId() const
{
    return 1;
}


DBManager *DBManager::getDBManager()
{
    if (!instance)
          instance = new DBManager();
    return instance;
}
