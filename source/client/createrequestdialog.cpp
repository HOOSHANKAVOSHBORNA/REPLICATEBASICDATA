#include "createrequestdialog.h"
#include "ui_createrequestdialog.h"
#include <QMenu>
#include<QSqlRecord>
#include <QSqlField>
#include <QDateTime>

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
    //m_dbm->openConnection();

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

QList<CreateRequestDialog::UpdateStruct> CreateRequestDialog::getUpdateIndexList() const
{
    return m_updateIndexList;
}

QString CreateRequestDialog::getDescription() const
{
    return ui->txtDescription->text();
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
    connect(m_model, SIGNAL(beforeUpdate(int, QSqlRecord &)), this, SLOT(onBeforeUpdate(int, QSqlRecord &)));

    QSqlRecord record = m_model->record();
    ui->tableView->hideColumn(record.indexOf("id"));
    //clear changed data----------
    m_insertIndexList.clear();
    m_deleteIndexList.clear();
}

void CreateRequestDialog::onInsertRow()
{
    QSqlRecord record = m_model->record();
    record.remove(record.indexOf("id"));
    qint64 time = QDateTime::currentMSecsSinceEpoch();
    record.setValue("created_at", time);
    if(m_model->insertRecord(-1, record))
    {
        m_insertIndexList.append(m_model->rowCount()- 1);
        //color row
        QModelIndex index = m_model->index(m_model->rowCount()- 1, 2);
        m_model->setData(index, QColor(Qt::red), Qt::BackgroundRole);
        ui->tableView->update();
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

void CreateRequestDialog::onBeforeUpdate(int row, QSqlRecord &record)
{
    QSqlRelationalTableModel *model = m_dbm->getRelationalModelTableName(ui->comboBox->currentText());
    m_updateIndexList.append({row, model->record(row), record});
}
