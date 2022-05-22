#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dbmanager.h"
#include <QDebug>
#include <QJsonDocument>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlQuery>
#include <QDateTime>
#include "createrequestdialog.h"
#include "senddialog.h"
#include <QSqlRelationalDelegate>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomMenuRequest(QPoint)));

    m_dbm  = DBManager::getDBManager();
    m_dbm->openConnection();

//    QSqlQueryModel* model = dbm.getRequestModel();
//    ui->tableView->setModel(model);

    m_requestModel = m_dbm->getRequestRelationalModel();
    ui->tableView->setModel(m_requestModel);
    ui->tableView->setItemDelegate(new QSqlRelationalDelegate(ui->tableView));
    ui->tableView->hideColumn(0);//id
    ui->tableView->hideColumn(7);//data

}

MainWindow::~MainWindow()
{
    //m_dbm->closeConnection();
    delete ui;
}

QByteArray MainWindow::toByteArray(QSqlRecord _rec)
{
    QByteArray result;
    QDataStream stream(&result,QIODevice::ReadWrite);
    stream.setVersion(QDataStream::Qt_5_13);
    for (int i = 0; i < _rec.count(); i++)
    {
        QSqlField field = _rec.field(i);
        stream << field.value();
    }
    return result;
}

QSqlRecord MainWindow::fromByteArray(QByteArray _data, QString _tableName)
{
    QDataStream stream(_data);
    stream.setVersion(QDataStream::Qt_5_13);
    QSqlRelationalTableModel *model = m_dbm->getRelationalModelTableName(_tableName);
    QSqlRecord rec = model->record();
    for (int i = 0; i < rec.count(); i++)
    {
        QVariant field;
        stream >> field;
        rec.setValue(i,field);
    }
    return rec;
}

void MainWindow::onCustomMenuRequest(QPoint pos)
{
    /* Create an object context menu */
   QMenu * menu = new QMenu(this);
   /* Create actions to the context menu */
   QAction * addRequest = new QAction("Add", this);
   QAction * sendRequest = new QAction("Send", this);
   QAction * rollbackRequest = new QAction("Rollback", this);
   QAction * refreshRequest = new QAction("Refresh", this);
   /* Connect slot handlers for Action pop-up menu */
   connect(addRequest, SIGNAL(triggered()), this, SLOT(onAddRequest()));  // Call Handler dialog editing
   connect(sendRequest, SIGNAL(triggered()), this, SLOT(onSendRequest()));
   connect(rollbackRequest, SIGNAL(triggered()), this, SLOT(onRollbackRequest()));
   connect(refreshRequest, SIGNAL(triggered()), this, SLOT(onRefreshRequest()));
   /* Set the actions to the menu */
   menu->addAction(addRequest);
   m_selectedRow = ui->tableView->rowAt(pos.y());
   if(m_selectedRow != -1)
   {
       menu->addAction(sendRequest);
       if(isRollbackable())
           menu->addAction(rollbackRequest);
   }
   menu->addAction(refreshRequest);
   /* Call the context menu */
   menu->popup(ui->tableView->viewport()->mapToGlobal(pos));
}

void MainWindow::onAddRequest()
{
    ///todo add transaction--------------------------------
    CreateRequestDialog *createRequest = new CreateRequestDialog(this);
    int ret = createRequest->exec();

    if(ret == QDialog::Accepted)
    {
        QSqlRelationalTableModel *model = createRequest->getModel();
        QList<int> insertIndexList = createRequest->getInsertIndexList();
        QList<CreateRequestDialog::DeleteStruct> deleteIndexList = createRequest->getDeleteIndexList();
        //--insert table changes---------------------------------------------
        if(!model->submitAll())
        {
            qDebug() << "onAddRequest:model:submit -> SQL ERROR: " << model->lastError().text();
            return;
        }
        //------
        //--insert "insert request" ----------------------------------------
        int tableIndex = m_dbm->getTableIndex(model->tableName());
        int type = m_dbm->getRequestTypeIndex("insert");
        int status = m_dbm->getRequestStatusIndex("checking");
        int applicant = m_dbm->getSelfId();
        int reviewer = m_dbm->getReviewerId();
        qint64 time = QDateTime::currentMSecsSinceEpoch();
        for(auto insertIndex:insertIndexList)
        {
            //valid index (delete index remove from model in submitAll)
            int index = insertIndex - deleteIndexList.length();
            QSqlRecord rec = model->record(index);
            QByteArray data = toByteArray(rec);
            QSqlRecord requestRec = m_requestModel->record();
            requestRec.remove(requestRec.indexOf("id"));
            requestRec.setValue(0, rec.field("id").value());
            requestRec.setValue(1, tableIndex);
            requestRec.setValue(2, applicant);
            requestRec.setValue(3, reviewer);
            requestRec.setValue(4, type);//insert
            requestRec.setValue(5, status);//checking
            requestRec.setValue(6, data);
            requestRec.setValue(7, "{}");
            requestRec.setValue(8, time);
            //qDebug() <<"requestRec:"<<requestRec;
            if(!m_requestModel->insertRecord(-1, requestRec))
            {
                m_requestModel->revertAll();
                return;
            }
        }
        //-------------------------------------------------------------------
        //--insert delete request--------------------------------------------
        type = m_dbm->getRequestTypeIndex("delete");
        for(auto delStruct:deleteIndexList)
        {
            //QSqlRecord rec = model->record(delStruct.index);
            QByteArray data = toByteArray(delStruct.rec);
            QSqlRecord requestRec = m_requestModel->record();
            requestRec.remove(requestRec.indexOf("id"));
            requestRec.setValue(0, delStruct.rec.field("id").value());
            requestRec.setValue(1, tableIndex);
            requestRec.setValue(2, applicant);
            requestRec.setValue(3, reviewer);
            requestRec.setValue(4, type);//delete
            requestRec.setValue(5, status);//checking
            requestRec.setValue(6, data);
            requestRec.setValue(7, "{}");
            requestRec.setValue(8, time);
            //qDebug() <<"requestRec:"<<requestRec;
            if(!m_requestModel->insertRecord(-1, requestRec))
            {
                m_requestModel->revertAll();
                return;
            }
        }
        //-------------------------------------------------------------------
        //--insert requests----------------------------------------------------------------
        if(!m_requestModel->submitAll())
        {
            qDebug() << "onAddRequest:m_requestModel:submit -> SQL ERROR: " << m_requestModel->lastError().text();
            m_requestModel->revertAll();
        }
    }

}

bool MainWindow::isRollbackable()
{
    QSqlRecord reqRec = m_requestModel->record(m_selectedRow);
    QString status = reqRec.value(6).toString();
    if(status == "checking" || status == "rejected")
        return true;
    return false;
}
void MainWindow::onRollbackRequest()
{
    QSqlRecord reqRec = m_requestModel->record(m_selectedRow);
    QString tableName = reqRec.value(2).toString();
    QString type = reqRec.value(5).toString();
    QByteArray data = reqRec.value("data").toByteArray();
    QSqlRecord tableRec = fromByteArray(data, tableName);
//    qDebug()<< reqRec;
//    qDebug()<< tableRec;
    QSqlRelationalTableModel *model = m_dbm->getRelationalModelTableName(tableName);
    if(!isRollbackable())
        return;
    if(type == "insert")
    {
        model->setFilter("id = " + tableRec.value("id").toString());
        model->select();
        if(model->removeRows(0, 1))
        {
            if(!model->submitAll())
            {
                qDebug() << "onRollbackRequest:model:insert -> SQL ERROR: " << model->lastError().text();
                return;
            }
        }
    }
    else if(type == "delete")
    {
        if(model->insertRecord(-1, tableRec))
        {
            if(!model->submitAll())
            {
                qDebug() << "onRollbackRequest:model:delete -> SQL ERROR: " << model->lastError().text();
                return;
            }
        }
    }
    if(m_requestModel->removeRows(m_selectedRow, 1))
    {
        if(!m_requestModel->submitAll())
        {
            qDebug() << "onRollbackRequest:m_requestModel:remove -> SQL ERROR: " << m_requestModel->lastError().text();
            m_requestModel->revertAll();
        }
        m_requestModel->select();
    }

}

void MainWindow::onSendRequest()
{
    // add acknowledgment --------------------------------------------------------------
    QSqlRecord reqRec = m_requestModel->record(m_selectedRow);
    QSqlRelationalTableModel *model = m_dbm->getRelationalModelTableName("acknowledgment");
    int reviewerId = m_dbm->getReviewerId();
    int selfId = m_dbm->getSelfId();
    int ackStatus = m_dbm->getAckStatusIndex("pending");
    if(selfId == reviewerId)//is center
    {
        SendDialog *sendDialog = new SendDialog(this);
        int ret = sendDialog->exec();

        if(ret == QDialog::Accepted)
        {
            QList<int> selectedIds = sendDialog->getSelectedId();
            for(int id:selectedIds)
            {
                QSqlRecord rec = model->record();
                rec.remove(rec.indexOf("id"));
                rec.setValue("request_id",reqRec.value("id"));
                rec.setValue("receiver",id);
                rec.setValue("status",ackStatus);
                model->insertRecord(-1, rec);
            }
        }
        else
            return;
    }
    else//is client and send to center
    {
        QSqlRecord rec = model->record();
        rec.remove(rec.indexOf("id"));
        rec.setValue("request_id",reqRec.value("id"));
        rec.setValue("receiver",reviewerId);
        rec.setValue("status",ackStatus);
        model->insertRecord(-1, rec);
    }
    if(!model->submitAll())
    {
        qDebug() << "onSendRequest:model:insert ack -> SQL ERROR: " << model->lastError().text();
        return;
    }
    //update request status-------------------------------------
    int reqStatus = m_dbm->getRequestStatusIndex("waiting");
    QSqlRecord curentRec = m_requestModel->record(m_selectedRow);
    curentRec.setValue(6, reqStatus);
    m_requestModel->setRecord(m_selectedRow, curentRec);
    if(!m_requestModel->submitAll())
    {
        qDebug() << "onSendRequest:m_requestModel:submit -> SQL ERROR: " << m_requestModel->lastError().text();
        m_requestModel->revertAll();
    }
}

void MainWindow::onRefreshRequest()
{
    m_requestModel->select();
}

