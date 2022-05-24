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

private:
    Ui::ReviewDialog *ui;
    DBManager *m_dbm;
    QSqlRecord m_tableRec;
    QSqlRelationalTableModel* m_model;
};

#endif // REVIEWDIALOG_H
