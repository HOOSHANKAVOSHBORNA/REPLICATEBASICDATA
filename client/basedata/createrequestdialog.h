#ifndef CREATEREQUESTDIALOG_H
#define CREATEREQUESTDIALOG_H

#include <QDialog>
#include <QList>
#include "dbmanager.h"

namespace Ui {
class CreateRequestDialog;
}

class CreateRequestDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateRequestDialog(QWidget *parent = nullptr);
    QSqlRelationalTableModel* getModel()const;
    QList<int> getInsertIndexList()const;
    QList<int> getDeleteIndexList()const;
    ~CreateRequestDialog();
private slots:
    void onCustomMenuRequest(QPoint pos);
    void onComboCurrentIndexChanged(QString _tableName);
    void onInsertRow();
    void onDeleteRow();
private:
    Ui::CreateRequestDialog *ui;
    DBManager *m_dbm;
    QSqlRelationalTableModel* m_model;
    QList<int> m_insertIndexList;
    QList<int> m_deleteIndexList;
    int m_selectedRow;
};

#endif // CREATEREQUESTDIALOG_H
