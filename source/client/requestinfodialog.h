#ifndef REQUESTINFODIALOG_H
#define REQUESTINFODIALOG_H

#include <QDialog>
#include <QSqlRecord>

namespace Ui {
class RequestInfoDialog;
}

class RequestInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RequestInfoDialog(QSqlRecord rec, QWidget *parent = nullptr);
    ~RequestInfoDialog();

private:
    Ui::RequestInfoDialog *ui;
};

#endif // REQUESTINFODIALOG_H
