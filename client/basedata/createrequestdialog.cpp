#include "createrequestdialog.h"
#include "ui_createrequestdialog.h"
#include "dbmanager.h"

CreateRequestDialog::CreateRequestDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateRequestDialog)
{
    ui->setupUi(this);
    setWindowTitle("Edit table");

    DBManager *dbm = DBManager::getDBManager();
    dbm->openConnection();

    QSqlQueryModel* model = dbm->getTableNameModel();
    ui->comboBox->setModel(model);

}

CreateRequestDialog::~CreateRequestDialog()
{
    delete ui;
}
