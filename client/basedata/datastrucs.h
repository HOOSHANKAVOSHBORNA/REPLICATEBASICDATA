#ifndef DATASTRUCS_H
#define DATASTRUCS_H
#include <QString>
#include <QDataStream>

struct Request{
    qint64 id;
    qint64 table_id;
    qint32 table_name_id;
    qint32 applicant;
    qint32 reviewer;
    qint16 type;
    qint16 status;
    QByteArray data;
    QString description;
    qint64 created_at;
    QByteArray toByteArray()
    {
        QByteArray result;
        QDataStream stream(&result,QIODevice::ReadWrite);
        stream.setVersion(QDataStream::Qt_5_13);
        stream << id
         << table_id
         << table_name_id
         << applicant
         << reviewer
         << type
         << status
         << data
         << description
         << created_at;
        return result;
    }
    void fromByteArray(const QByteArray* reqData)
    {
        QDataStream stream(*reqData);
        stream.setVersion(QDataStream::Qt_5_13);
        stream >> id
         >> table_id
         >> table_name_id
         >> applicant
         >> reviewer
         >> type
         >> status
         >> data
         >> description
         >> created_at;
    }
};

#endif // DATASTRUCS_H
