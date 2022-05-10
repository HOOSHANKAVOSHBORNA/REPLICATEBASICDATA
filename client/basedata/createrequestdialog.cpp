#include "createrequestdialog.h"
#include "ui_createrequestdialog.h"
#include <QMenu>
#include<QSqlRecord>

CreateRequestDialog::CreateRequestDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateRequestDialog),
    dbm(DBManager::getDBManager())
{
    ui->setupUi(this);
    setWindowTitle("Edit table");
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    //connnect signals
    connect(ui->comboBox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(onComboCurrentIndexChanged(QString)));
    connect(ui->tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomMenuRequest(QPoint)));

    //dbm = DBManager::getDBManager();
    dbm->openConnection();

    QSqlQueryModel* model = dbm->getTableNameModel();
    ui->comboBox->setModel(model);


}

QSqlRelationalTableModel *CreateRequestDialog::getModel() const
{
    return model;
}

QList<int> CreateRequestDialog::getInsertIndexList() const
{
    return insertIndexList;
}

CreateRequestDialog::~CreateRequestDialog()
{
    delete ui;
}

void CreateRequestDialog::onCustomMenuRequest(QPoint pos)
{
    /* Create an object context menu */
   QMenu * menu = new QMenu(this);
   /* Create actions to the context menu */
   QAction * insertRow = new QAction("Insert Row", this);
   /* Connect slot handlers for Action pop-up menu */
   connect(insertRow, SIGNAL(triggered()), this, SLOT(onInsertRow()));  // Call Handler dialog editing
   /* Set the actions to the menu */
   menu->addAction(insertRow);
   /* Call the context menu */
   menu->popup(ui->tableView->viewport()->mapToGlobal(pos));
}

void CreateRequestDialog::onComboCurrentIndexChanged(QString _tableName)
{
    model = dbm->getRelationalModelTableName(_tableName);
    ui->tableView->setModel(model);
}

void CreateRequestDialog::onInsertRow()
{
    QSqlRecord record = model->record();
    record.remove(record.indexOf("id"));
    //record.setValue("firstname", f);
    if(model->insertRecord(-1, record))
    {
        insertIndexList.append(model->rowCount()- 1);
    }
}
