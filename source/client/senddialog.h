#ifndef SENDDIALOG_H
#define SENDDIALOG_H

#include "dbmanager.h"

#include <QDialog>

namespace Ui {
class SendDialog;
}

class SendDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SendDialog(QWidget *parent = nullptr);
    ~SendDialog();
    QList<int> getSelectedId() const;
private slots:
    void onCellClicked(int, int);

private:
    Ui::SendDialog *ui;
    DBManager *m_dbm;
};

#endif // SENDDIALOG_H
