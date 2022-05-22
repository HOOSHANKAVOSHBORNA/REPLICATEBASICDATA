#ifndef REVIEWDIALOG_H
#define REVIEWDIALOG_H

#include "dbmanager.h"

#include <QDialog>
#include <QSqlRecord>

namespace Ui {
class ReviewDialog;
}

class ReviewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReviewDialog(QSqlRecord rec, QWidget *parent = nullptr);
    ~ReviewDialog();

private:
    Ui::ReviewDialog *ui;
    DBManager *m_dbm;
};

#endif // REVIEWDIALOG_H
