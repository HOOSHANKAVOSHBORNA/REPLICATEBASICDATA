#include "senddialog.h"
#include "ui_senddialog.h"
#include <QSqlRecord>
#include <QCheckBox>
#include <QTableWidgetItem>

SendDialog::SendDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SendDialog),
    m_dbm(DBManager::getDBManager())
{
    ui->setupUi(this);
    setWindowTitle("Send Form");
    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->resizeColumnToContents(0);
    ui->tableWidget->resizeColumnToContents(1);
    connect(ui->tableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(onCellClicked(int, int)));
    //dbm = DBManager::getDBManager();
    m_dbm->openConnection();
    QSqlRelationalTableModel* model = m_dbm->getRelationalModelTableName("client_info");
    for (int i = 0; i < model->rowCount(); ++i)
    {
        int port_index = model->record(i).value("port_index").toInt();
        if(port_index != -1)
        {
            int id = model->record(i).value("id").toInt();
            QString name = model->record(i).value("name").toString();
            QTableWidgetItem *itemId = new QTableWidgetItem(QString::number(id));
            QTableWidgetItem *itemName = new QTableWidgetItem(name);
            QCheckBox *checkBox = new QCheckBox();
            int row = ui->tableWidget->rowCount();
            ui->tableWidget->insertRow(row);
            ui->tableWidget->setCellWidget(row,0,checkBox);
            ui->tableWidget->setItem(row, 1, itemId);
            ui->tableWidget->setItem(row, 2, itemName);
        }
    }
}

SendDialog::~SendDialog()
{
    delete ui;
}

QList<int> SendDialog::getSelectedId() const
{
    QList<int> results;
    for (int i = 0; i < ui->tableWidget->rowCount(); ++i)
    {
        QCheckBox *checkBox = static_cast<QCheckBox*>(ui->tableWidget->cellWidget(i, 0));
        if(checkBox->isChecked())
        {
            int id = ui->tableWidget->item(i,1)->text().toInt();
            results.append(id);
        }
    }
    return results;
}

void SendDialog::onCellClicked(int row, int)
{
    QCheckBox *checkBox = static_cast<QCheckBox*>(ui->tableWidget->cellWidget(row, 0));
    checkBox->setChecked(!checkBox->isChecked());
}
