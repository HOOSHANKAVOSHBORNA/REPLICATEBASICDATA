#ifndef REVIEWDIALOG_H
#define REVIEWDIALOG_H

#include <QDialog>
#include <QSqlRecord>

#include "../common/dbmanager.h"

namespace Ui {
class ReviewDialog;
}

class ReviewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReviewDialog(QSqlRecord rec, QWidget *parent = nullptr);
    ~ReviewDialog();
    QString getDescription() const;
    QSqlRecord getEditRecord() const;
    bool hasEditRecord() const;
private:
    void filter();
private slots:
    void onCellClicked(int, int);
    void onStateChanged(int);
    void onTextChanged(const QString &);
    void onCurrentIndexChanged(int);
    void on_btnAccept_clicked();
    void on_btnReject_clicked();
    void on_btnClose_clicked();
    void onCustomMenuRequest(QPoint pos);
    void onInsertEditRow();
    void onDeleteEditRow();

private:
    Ui::ReviewDialog *ui;
    DBManager *m_dbm;
    QSqlRecord m_tableRec;
    QSqlRelationalTableModel* m_model;
    bool m_hasEditRow;
    QString m_reqType;
    QString m_reqStatus;
};

#endif // REVIEWDIALOG_H
