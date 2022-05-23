#include "packetmanager.h"
#include "reviewdialog.h"
#include "ui_reviewdialog.h"

#include <QCheckBox>

ReviewDialog::ReviewDialog(QSqlRecord rec, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReviewDialog)
{
    ui->setupUi(this);
    setWindowTitle("Review Request");

    m_dbm  = DBManager::getDBManager();
    m_dbm->openConnection();

    QString tableName = rec.value(2).toString();
    QString applicant = rec.value(3).toString();
    QString type = rec.value(5).toString();
    QByteArray data = rec.value("data").toByteArray();
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
