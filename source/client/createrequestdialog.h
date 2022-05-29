#ifndef CREATEREQUESTDIALOG_H
#define CREATEREQUESTDIALOG_H

#include <QDialog>
#include <QList>
#include <QSqlRecord>
#include "../common/dbmanager.h"

namespace Ui {
class CreateRequestDialog;
}

class CreateRequestDialog : public QDialog
{
    Q_OBJECT
public:
    struct DeleteStruct
    {
        int index;
        QSqlRecord rec;
    };
    struct UpdateStruct
    {
        int index;
        QSqlRecord oldRec;
        QSqlRecord newRec;
    };
public:
    explicit CreateRequestDialog(QWidget *parent = nullptr);
    QSqlRelationalTableModel* getModel()const;
    QList<int> getInsertIndexList()const;
    QList<DeleteStruct> getDeleteIndexList()const;
    QList<UpdateStruct> getUpdateIndexList()const;
    QString getDescription()const;
    ~CreateRequestDialog();
private slots:
    void onCustomMenuRequest(QPoint pos);
    void onComboCurrentIndexChanged(QString _tableName);
    void onInsertRow();
    void onDeleteRow();
    void onBeforeUpdate(int row, QSqlRecord &record);
private:
    Ui::CreateRequestDialog *ui;
    DBManager *m_dbm;
    QSqlRelationalTableModel* m_model;
    QList<int> m_insertIndexList;
    QList<DeleteStruct> m_deleteIndexList;
    QList<UpdateStruct> m_updateIndexList;
    int m_selectedRow;
};

#endif // CREATEREQUESTDIALOG_H
