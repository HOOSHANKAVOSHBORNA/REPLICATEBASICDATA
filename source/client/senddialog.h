#ifndef SENDDIALOG_H
#define SENDDIALOG_H

#include <QDialog>

#include "../common/dbmanager.h"

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
