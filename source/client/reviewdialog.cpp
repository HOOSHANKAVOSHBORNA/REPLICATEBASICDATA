
#include "reviewdialog.h"
#include "ui_reviewdialog.h"
#include "../common/packetmanager.h"
#include <QCheckBox>
#include <QMenu>

ReviewDialog::ReviewDialog(QSqlRecord rec, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReviewDialog),
    m_hasEditRow(false)
{
    ui->setupUi(this);
    setWindowTitle("Review Request");

    ui->tableWidgetReq->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableWidgetReq, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomMenuRequest(QPoint)));

    m_dbm  = DBManager::getDBManager();
    //m_dbm->openConnection();

    QString tableName = rec.value(2).toString();
    QString applicant = rec.value(3).toString();
    QString type = rec.value(5).toString();
    m_reqType = type;
    m_reqStatus = rec.value(6).toString();
    QByteArray data = rec.value("data").toByteArray();

    if(type == "update")
    {
        QList<QByteArray> datas = PacketManager::fromByteArray(data);
        QSqlRecord oldRec = PacketManager::fromByteArray(datas.at(0), m_dbm, tableName);
        QSqlRecord newRec = PacketManager::fromByteArray(datas.at(1), m_dbm, tableName);

        m_tableRec = newRec;
        ui->tableWidgetReq->setRowCount(2);
        ui->tableWidgetReq->setColumnCount(m_tableRec.count());
        for(int i = 0; i <  m_tableRec.count(); ++i)
        {
            QString name = m_tableRec.fieldName(i);
            QString value = m_tableRec.value(i).toString();
            QCheckBox* checkbox = new QCheckBox();
            checkbox->setText(value);
            ui->tableWidgetReq->setCellWidget(0, i, checkbox);
            ui->tableWidgetReq->setHorizontalHeaderItem(i,new QTableWidgetItem(name));
            ui->comboField->addItem(name);

            connect(checkbox, SIGNAL(stateChanged(int)), this, SLOT(onStateChanged(int)));
        }
        for(int i = 0; i <  oldRec.count(); ++i)
        {
            QString value = oldRec.value(i).toString();
            ui->tableWidgetReq->setItem(1, i, new QTableWidgetItem(value));
        }

        ui->tableWidgetReq->setVerticalHeaderItem(0,new QTableWidgetItem("Current"));
        ui->tableWidgetReq->setVerticalHeaderItem(1,new QTableWidgetItem("Old"));
    }
    else
    {
        m_tableRec = PacketManager::fromByteArray(data, m_dbm, tableName);
        ui->tableWidgetReq->setRowCount(1);
        ui->tableWidgetReq->setColumnCount(m_tableRec.count());
        for(int i = 0; i <  m_tableRec.count(); ++i)
        {
            QString name = m_tableRec.fieldName(i);
            QString value = m_tableRec.value(i).toString();
            QCheckBox* checkbox = new QCheckBox();
            checkbox->setText(value);
            ui->tableWidgetReq->setCellWidget(0, i, checkbox);
            ui->tableWidgetReq->setHorizontalHeaderItem(i,new QTableWidgetItem(name));
            ui->comboField->addItem(name);

            connect(checkbox, SIGNAL(stateChanged(int)), this, SLOT(onStateChanged(int)));
        }
        ui->tableWidgetReq->setVerticalHeaderItem(0,new QTableWidgetItem("Current"));
    }
    ui->tableWidgetReq->hideColumn(0);
    ui->comboField->removeItem(0);

    m_model = m_dbm->getRelationalModelTableName(tableName);
    ui->tableViewBase->setModel(m_model);
    ui->tableViewBase->hideColumn(0);

    ui->txtType->setText(type);
    ui->txtTable->setText(tableName);
    ui->txtApplicant->setText(applicant);

    connect(ui->tableWidgetReq, SIGNAL(cellClicked(int, int)), this, SLOT(onCellClicked(int, int)));
    connect(ui->txtSearch, SIGNAL(textChanged(const QString &)), this, SLOT(onTextChanged(const QString &)));
    connect(ui->comboField, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
}

ReviewDialog::~ReviewDialog()
{
    delete ui;
}

QString ReviewDialog::getDescription() const
{
    return ui->txtDescription->text();
}

QSqlRecord ReviewDialog::getEditRecord() const
{
    QSqlRecord rec = m_model->record();
    if(m_hasEditRow)
    {
        int row = ui->tableWidgetReq->rowCount()-1;
        for(int i = 0; i <  ui->tableWidgetReq->columnCount(); ++i)
        {
            QString itemText = ui->tableWidgetReq->item(row, i)->text();
            rec.setValue(i,itemText);
        }
    }
    return rec;
}

bool ReviewDialog::hasEditRecord() const
{
    return m_hasEditRow;
}

void ReviewDialog::filter()
{
    QString strFilter = "";
    for(int i = 0; i <  m_tableRec.count(); ++i)
    {
        QString name = m_tableRec.fieldName(i);
        QVariant value = m_tableRec.value(i);
        QCheckBox* checkbox = static_cast<QCheckBox*>(ui->tableWidgetReq->cellWidget(0, i));
        if(checkbox->isChecked())
        {
            if(value.type() == QVariant::Type::String)
                strFilter =  strFilter + name + " like '%" + value.toString() + "%' AND ";
            else
                strFilter =  strFilter + name + " = " + value.toString() + " AND ";
        }
    }
    strFilter +=  " 1 = 1 ";

    m_model->setFilter(strFilter);
    m_model->select();
}

void ReviewDialog::onCellClicked(int row, int col)
{
    if(row != 0) return;
    QCheckBox *checkBox = static_cast<QCheckBox*>(ui->tableWidgetReq->cellWidget(row, col));
    checkBox->setChecked(!checkBox->isChecked());
}

void ReviewDialog::onStateChanged(int)
{
    filter();
}

void ReviewDialog::onTextChanged(const QString & txt)
{
    QString curentName = ui->comboField->currentText();
    QVariant::Type type = m_tableRec.value(curentName).type();
    QString strFilter;
    if(type == QVariant::Type::String)
        strFilter = curentName + " like '%"+ txt + "%'";
    else
        strFilter = curentName + " = "+ txt;
    if(txt.isEmpty())
        strFilter = "1 = 1";
    m_model->setFilter(strFilter);
    m_model->select();
}

void ReviewDialog::onCurrentIndexChanged(int)
{
    ui->txtSearch->setText("");
}

void ReviewDialog::on_btnAccept_clicked()
{
    accept();
}

void ReviewDialog::on_btnReject_clicked()
{
    done(-1);
}

void ReviewDialog::on_btnClose_clicked()
{
    close();
}

void ReviewDialog::onCustomMenuRequest(QPoint pos)
{
    //can not edit delete request
    if(m_reqType != "delete" && m_reqStatus != "checking")
    {
        /* Create an object context menu */
       QMenu * menu = new QMenu(this);
       /* Create actions to the context menu */
       QAction * insertRow = new QAction("Edit Request", this);
       QAction * deleteRow = new QAction("Delete Edit Request", this);
       /* Connect slot handlers for Action pop-up menu */
       connect(insertRow, SIGNAL(triggered()), this, SLOT(onInsertEditRow()));  // Call Handler dialog editing
       connect(deleteRow, SIGNAL(triggered()), this, SLOT(onDeleteEditRow()));
       /* Set the actions to the menu */
       menu->addAction(insertRow);
       //int row = ui->tableWidgetReq->rowAt(pos.y());
       menu->addAction(deleteRow);
       /* Call the context menu */
       menu->popup(ui->tableWidgetReq->viewport()->mapToGlobal(pos));
    }
}

void ReviewDialog::onInsertEditRow()
{
    if(!m_hasEditRow)
    {
        int row = ui->tableWidgetReq->rowCount();
        ui->tableWidgetReq->insertRow(row);
        for(int i = 0; i <  m_tableRec.count(); ++i)
        {
            QString value = m_tableRec.value(i).toString();
            ui->tableWidgetReq->setItem(row, i, new QTableWidgetItem(value));
        }

        ui->tableWidgetReq->setVerticalHeaderItem(row,new QTableWidgetItem("edit"));

        m_hasEditRow = true;
    }
}

void ReviewDialog::onDeleteEditRow()
{
    if(m_hasEditRow)
    {
        int row = ui->tableWidgetReq->rowCount();
        ui->tableWidgetReq->removeRow(row - 1);
        m_hasEditRow = false;
    }
}
