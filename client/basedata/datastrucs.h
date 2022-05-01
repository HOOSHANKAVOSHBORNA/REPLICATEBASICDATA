#ifndef DATASTRUCS_H
#define DATASTRUCS_H
#include <QString>
#include <QJsonObject>

struct request{
    int64_t id;
    int64_t table_id;
    int table_name_id;
    int applicant;
    int reviewer;
    int16_t type;
    int16_t status;
    QByteArray data;
    QString description;
    int64_t created_at;
};

#endif // DATASTRUCS_H
