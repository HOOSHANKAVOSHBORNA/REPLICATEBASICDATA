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
    ~CreateRequestDialog();
private slots:
    void onCustomMenuRequest(QPoint pos);
    void onComboCurrentIndexChanged(QString _tableName);
    void onInsertRow();
private:
    Ui::CreateRequestDialog *ui;
    DBManager *dbm;
    QSqlRelationalTableModel* model;
    QList<int> insertIndexList;
};

#endif // CREATEREQUESTDIALOG_H
