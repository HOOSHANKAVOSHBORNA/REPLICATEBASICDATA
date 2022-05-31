#include "requestinfodialog.h"
#include "ui_requestinfodialog.h"

#include <dbmanager.h>
#include <packetmanager.h>

RequestInfoDialog::RequestInfoDialog(QSqlRecord rec, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RequestInfoDialog)
{
    ui->setupUi(this);
    setWindowTitle("Request Info");

    DBManager *dbm  = DBManager::getDBManager();
    //m_dbm->openConnection();

    QString tableName = rec.value(2).toString();
    QString applicant = rec.value(3).toString();
    QString type = rec.value(5).toString();
    QByteArray data = rec.value("data").toByteArray();

    if(type == "update")
    {
        QList<QByteArray> datas = PacketManager::fromByteArray(data);
        QSqlRecord oldRec = PacketManager::fromByteArray(datas.at(0), dbm, tableName);
        QSqlRecord newRec = PacketManager::fromByteArray(datas.at(1), dbm, tableName);

        ui->tableWidget->setRowCount(2);
        ui->tableWidget->setColumnCount(newRec.count());
        for(int i = 0; i <  newRec.count(); ++i)
        {
            QString name = newRec.fieldName(i);
            ui->tableWidget->setHorizontalHeaderItem(i,new QTableWidgetItem(name));

            QString value = newRec.value(i).toString();
            ui->tableWidget->setItem(0, i, new QTableWidgetItem(value));
        }
        for(int i = 0; i <  oldRec.count(); ++i)
        {
            QString value = oldRec.value(i).toString();
            ui->tableWidget->setItem(1, i, new QTableWidgetItem(value));
        }

        ui->tableWidget->setVerticalHeaderItem(0,new QTableWidgetItem("Current"));
        ui->tableWidget->setVerticalHeaderItem(1,new QTableWidgetItem("Old"));
    }
    else
    {
        QSqlRecord rec = PacketManager::fromByteArray(data, dbm, tableName);
        ui->tableWidget->setRowCount(1);
        ui->tableWidget->setColumnCount(rec.count());

        for(int i = 0; i <  rec.count(); ++i)
        {
            QString name = rec.fieldName(i);
            ui->tableWidget->setHorizontalHeaderItem(i,new QTableWidgetItem(name));

            QString value = rec.value(i).toString();
            ui->tableWidget->setItem(0, i, new QTableWidgetItem(value));
        }
        ui->tableWidget->setVerticalHeaderItem(0,new QTableWidgetItem("Current"));
    }
    ui->tableWidget->hideColumn(0);

    ui->txtType->setText(type);
    ui->txtTable->setText(tableName);
    ui->txtApplicant->setText(applicant);
}

RequestInfoDialog::~RequestInfoDialog()
{
    delete ui;
}
