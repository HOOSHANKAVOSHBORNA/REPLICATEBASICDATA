#ifndef CREATEREQUESTDIALOG_H
#define CREATEREQUESTDIALOG_H

#include <QDialog>

namespace Ui {
class CreateRequestDialog;
}

class CreateRequestDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateRequestDialog(QWidget *parent = nullptr);
    ~CreateRequestDialog();

private:
    Ui::CreateRequestDialog *ui;
};

#endif // CREATEREQUESTDIALOG_H
