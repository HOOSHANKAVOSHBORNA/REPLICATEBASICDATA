#include "createrequestdialog.h"
#include "ui_createrequestdialog.h"
#include <QMenu>
#include<QSqlRecord>
#include <QSqlField>

CreateRequestDialog::CreateRequestDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateRequestDialog),
    m_dbm(DBManager::getDBManager())
{
    ui->setupUi(this);
    setWindowTitle("Edit table");
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    //connnect signals
    connect(ui->comboBox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(onComboCurrentIndexChanged(QString)));
    connect(ui->tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomMenuRequest(QPoint)));

    //dbm = DBManager::getDBManager();
    m_dbm->openConnection();

    QSqlQueryModel* model = m_dbm->getTableNameModel();
    ui->comboBox->setModel(model);


}

QSqlRelationalTableModel *CreateRequestDialog::getModel() const
{
    return m_model;
}

QList<int> CreateRequestDialog::getInsertIndexList() const
{
    return m_insertIndexList;
}

QList<CreateRequestDialog::DeleteStruct> CreateRequestDialog::getDeleteIndexList() const
{
    return m_deleteIndexList;
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
   QAction * deleteRow = new QAction("Delete Row", this);
   /* Connect slot handlers for Action pop-up menu */
   connect(insertRow, SIGNAL(triggered()), this, SLOT(onInsertRow()));  // Call Handler dialog editing
   connect(deleteRow, SIGNAL(triggered()), this, SLOT(onDeleteRow()));
   /* Set the actions to the menu */
   menu->addAction(insertRow);
   m_selectedRow = ui->tableView->rowAt(pos.y());
   if(m_selectedRow != -1)
   {
       menu->addAction(deleteRow);
   }
   /* Call the context menu */
   menu->popup(ui->tableView->viewport()->mapToGlobal(pos));
}

void CreateRequestDialog::onComboCurrentIndexChanged(QString _tableName)
{
    m_model = m_dbm->getRelationalModelTableName(_tableName);
    ui->tableView->setModel(m_model);
    //clear changed data----------
    m_insertIndexList.clear();
    m_deleteIndexList.clear();
}

void CreateRequestDialog::onInsertRow()
{
    QSqlRecord record = m_model->record();
    record.remove(record.indexOf("id"));
    //record.setValue("firstname", f);
    if(m_model->insertRecord(-1, record))
    {
        m_insertIndexList.append(m_model->rowCount()- 1);
    }
}

void CreateRequestDialog::onDeleteRow()
{
    bool isInInsert = false;
    if(m_model->removeRows(m_selectedRow, 1))
    {
        for(int i = 0; i < m_insertIndexList.count(); i++)
        {
            if(m_insertIndexList.at(i) == m_selectedRow)
            {
                m_insertIndexList.removeAt(i);
                isInInsert = true;
                for(int j = i; j<m_insertIndexList.count();j++)
                    m_insertIndexList[j]--;
                break;
            }
        }
        if(!isInInsert)
        {
            m_deleteIndexList.append({m_selectedRow,m_model->record(m_selectedRow)});
        }
    }
}
