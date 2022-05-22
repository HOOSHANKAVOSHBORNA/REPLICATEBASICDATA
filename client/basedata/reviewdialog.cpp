#include "packetmanager.h"
#include "reviewdialog.h"
#include "ui_reviewdialog.h"

#include <QCheckBox>

ReviewDialog::ReviewDialog(QSqlRecord rec, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReviewDialog)
{
    ui->setupUi(this);

    m_dbm  = DBManager::getDBManager();
    m_dbm->openConnection();

    QString tableName = rec.value(2).toString();
    QString type = rec.value(5).toString();
    QByteArray data = rec.value("data").toByteArray();
    QSqlRecord tableRec = PacketManager::fromByteArray(data, m_dbm, tableName);

    ui->tableWidgetReq->setRowCount(1);
    ui->tableWidgetReq->setColumnCount(tableRec.count());
    for(int i = 0; i <  tableRec.count(); ++i)
    {
        QString name = tableRec.fieldName(i);
        QString value = tableRec.value(i).toString();
        QCheckBox* checkbox = new QCheckBox();
        checkbox->setText(value);
        ui->tableWidgetReq->setCellWidget(0, i, checkbox);
        ui->tableWidgetReq->setHorizontalHeaderItem(i,new QTableWidgetItem(name));
    }
    ui->tableWidgetReq->hideColumn(0);

    QSqlRelationalTableModel *modelBase = m_dbm->getRelationalModelTableName(tableName);
    ui->tableViewBase->setModel(modelBase);
    ui->tableViewBase->hideColumn(0);
    ui->txtType->setText(type);
    ui->txtTable->setText(tableName);
}

ReviewDialog::~ReviewDialog()
{
    delete ui;
}
