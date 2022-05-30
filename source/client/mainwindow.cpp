#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QJsonDocument>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlQuery>
#include <QDateTime>
#include "createrequestdialog.h"
#include "reviewdialog.h"
#include "senddialog.h"
#include <QSqlRelationalDelegate>
#include "../common/packetmanager.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_dbm(DBManager::getDBManager())
{
    ui->setupUi(this);

    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomMenuRequest(QPoint)));

    m_requestModel = m_dbm->getRequestRelationalModel();
    ui->tableView->setModel(m_requestModel);
    ui->tableView->setItemDelegate(new QSqlRelationalDelegate(ui->tableView));
    ui->tableView->hideColumn(0);//id
    ui->tableView->hideColumn(7);//data
    ui->tableView->hideColumn(11);//apply

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onCustomMenuRequest(QPoint pos)
{
    //update selected row
    m_selectedRow = ui->tableView->rowAt(pos.y());
    /* Create an object context menu */
   QMenu * menu = new QMenu(this);
   /* Create actions to the context menu */
   QAction * addRequest = new QAction("Add", this);
   QAction * sendRequest = new QAction("Send", this);
   QAction * deletRequest = new QAction("Delete", this);
   QAction * refreshRequest = new QAction("Refresh", this);
   QAction * reviewRequest = new QAction("Review", this);
   QAction * applyRequest = new QAction("Apply", this);
   /* Connect slot handlers for Action pop-up menu */
   connect(addRequest, SIGNAL(triggered()), this, SLOT(onAddRequest()));
   connect(sendRequest, SIGNAL(triggered()), this, SLOT(onSendRequest()));
   connect(deletRequest, SIGNAL(triggered()), this, SLOT(onDeleteRequest()));
   connect(refreshRequest, SIGNAL(triggered()), this, SLOT(onRefreshRequest()));
   connect(reviewRequest, SIGNAL(triggered()), this, SLOT(onReviewRequest()));
   connect(applyRequest, SIGNAL(triggered()), this, SLOT(onApplyRequest()));
   /* Set the actions to the menu */
   menu->addAction(addRequest);
   menu->addAction(refreshRequest);
   // disable action menu
   if(m_selectedRow != -1)
   {
       menu->addAction(sendRequest);
       menu->addAction(deletRequest);
       menu->addAction(reviewRequest);
       menu->addAction(applyRequest);
       if(!isSendable())
           sendRequest->setDisabled(true);
       if(!isDeleteable())
           deletRequest->setDisabled(true);
       if(!isReviewable())
           reviewRequest->setDisabled(true);
       if(!isApplyable())
           applyRequest->setDisabled(true);
   }
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
        QString description = createRequest->getDescription();
        //--insert table changes---------------------------------------------
        if(!model->submitAll())
        {
            qDebug() << "onAddRequest:model:submit -> SQL ERROR: " << model->lastError().text();
            return;
        }
        QList<CreateRequestDialog::UpdateStruct> updateIndexList = createRequest->getUpdateIndexList();
        //------
        //--insert "insert request" ----------------------------------------

        for(auto insertIndex:insertIndexList)
        {
            //valid index (delete index remove from model in submitAll)
            int index = insertIndex - deleteIndexList.length();
            QSqlRecord rec = model->record(index);
            addRequest("insert", description, rec, rec);
        }
        //-------------------------------------------------------------------
        //--insert delete request--------------------------------------------
        for(auto delStruct:deleteIndexList)
        {
            addRequest("delete", description, delStruct.rec, delStruct.rec);
        }
        //-------------------------------------------------------------------
        //--insert update request--------------------------------------------
        for(auto updateStruct:updateIndexList)
        {
            addRequest("update", description, updateStruct.newRec, updateStruct.oldRec);
        }
        //-------------------------------------------------------------------
        //--submit changes to db --------------------------------------------
        if(!m_requestModel->submitAll())
        {
            qDebug() << "onAddRequest:m_requestModel:submit -> SQL ERROR: " << m_requestModel->lastError().text();
            m_requestModel->revertAll();
        }
    }

}

bool MainWindow::isDeleteable()
{
    QSqlRecord reqRec = m_requestModel->record(m_selectedRow);
    QString status = reqRec.value(6).toString();
    if(status == "checking")
        return true;
    return false;
}

bool MainWindow::isReviewable()
{
    QSqlRecord reqRec = m_requestModel->record(m_selectedRow);
    int requestId = reqRec.value("id").toInt();
    if(!m_dbm->isReviewer())
        return false;
    if(m_dbm->isSended(requestId))
        return false;
    if(m_dbm->hasApplied(requestId))
        return false;

    return true;
}

bool MainWindow::isSendable()
{
    QSqlRecord reqRec = m_requestModel->record(m_selectedRow);
    int requestId = reqRec.value("id").toInt();
    QString status = reqRec.value(6).toString();
    bool isReviewer = m_dbm->isReviewer();
    bool isApplicant = m_dbm->isApplicant(requestId);

    if( isReviewer
       &&(status == "checking" || status == "waiting"))//reviewer can not send checking and waiting request
        return false;
    if(!isReviewer && status != "checking")//clients can send only checking request
        return false;
    if(!isReviewer && m_dbm->isSended(requestId))// for reviewer checked in send dialog form
        return false;
    if(isApplicant && status == "rejected")
        return false;
    return true;
}

bool MainWindow::isApplyable()
{
    QSqlRecord reqRec = m_requestModel->record(m_selectedRow);
    int requestId = reqRec.value("id").toInt();
    QString status = reqRec.value(6).toString();
    if(status == "checking" || status == "waiting")
        return false;
    if(m_dbm->hasApplied(requestId))
        return false;
    bool isApplicant = m_dbm->isApplicant(requestId);
    if(isApplicant && status == "accepted")
        return false;
    if(!isApplicant && status == "rejected")
        return false;
    return true;
}

void MainWindow::insertRow(const QSqlRecord &tableRec, QSqlRelationalTableModel *model)
{
    if(model->insertRecord(-1, tableRec))
    {
        if(!model->submitAll())
        {
            qDebug() << "insertRow:model:insert -> SQL ERROR: " << model->lastError().text();
        }
    }
}

void MainWindow::deleteRow(const QSqlRecord &tableRec, QSqlRelationalTableModel *model)
{
    model->setFilter("id = " + tableRec.value("id").toString());
    model->select();
    if(model->removeRows(0, 1))
    {
        if(!model->submitAll())
        {
            qDebug() << "deleteRow:model:delete -> SQL ERROR: " << model->lastError().text();
        }
    }
}

void MainWindow::updateRow(const QSqlRecord &oldRec, const QSqlRecord &newRec, QSqlRelationalTableModel *model)
{
    model->setFilter("id = " + newRec.value("id").toString());
    model->select();
    if(model->setRecord(0, oldRec))
    {
        if(!model->submitAll())
        {
            qDebug() << "updateRow:model:update -> SQL ERROR: " << model->lastError().text();
        }
    }
}

void MainWindow::addRequest(QString typeStr, QString description, const QSqlRecord& rec, const QSqlRecord& recOld)
{
    QByteArray data;
    if(typeStr == "update")
    {
        //QSqlRecord rec = model->record(delStruct.index);
        QByteArray oldData = PacketManager::toByteArray(recOld);
        QByteArray newData = PacketManager::toByteArray(rec);
        data = PacketManager::toByteArray(oldData, newData);
    }
    else
    {
        data = PacketManager::toByteArray(rec);
    }

    int tableIndex = m_dbm->getTableIndex(rec.field(0).tableName());
    int type = m_dbm->getRequestTypeIndex(typeStr);
    int status = m_dbm->getRequestStatusIndex("checking");
    int applicant = m_dbm->getSelfId();
    int reviewer = m_dbm->getReviewerId();
    qint64 time = QDateTime::currentMSecsSinceEpoch();

    QSqlRecord requestRec = m_requestModel->record();
    requestRec.remove(requestRec.indexOf("id"));
    requestRec.setValue(0, rec.field("id").value());
    requestRec.setValue(1, tableIndex);
    requestRec.setValue(2, applicant);
    requestRec.setValue(3, reviewer);
    requestRec.setValue(4, type);
    requestRec.setValue(5, status);
    requestRec.setValue(6, data);
    requestRec.setValue(7, time);
    requestRec.setValue(8, description);
    if(!m_requestModel->insertRecord(-1, requestRec))
    {
        m_requestModel->revertAll();
    }
}
void MainWindow::onDeleteRequest()
{
    QSqlRecord reqRec = m_requestModel->record(m_selectedRow);
    QString tableName = reqRec.value(2).toString();
    QString type = reqRec.value(5).toString();
    QByteArray data = reqRec.value("data").toByteArray();

    QSqlRelationalTableModel *model = m_dbm->getRelationalModelTableName(tableName);
    if(type == "insert")
    {
        QSqlRecord tableRec = PacketManager::fromByteArray(data, m_dbm, tableName);
        deleteRow(tableRec, model);
    }
    else if(type == "delete")
    {
        QSqlRecord tableRec = PacketManager::fromByteArray(data, m_dbm, tableName);
        insertRow(tableRec, model);
    }
    else if(type == "update")
    {
        QList<QByteArray> datas = PacketManager::fromByteArray(data);
        QSqlRecord oldRec = PacketManager::fromByteArray(datas.at(0), m_dbm, tableName);
        QSqlRecord newRec = PacketManager::fromByteArray(datas.at(1), m_dbm, tableName);
        updateRow(oldRec, newRec, model);
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
    int ackStatus = m_dbm->getAckStatusIndex("pending");
    int requestId = reqRec.value("id").toInt();
    if(m_dbm->isReviewer())
    {
        SendDialog *sendDialog = new SendDialog(requestId, this);
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
    else//is client and send to reviewer
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
    QSqlRecord curentRec = m_requestModel->record(m_selectedRow);
    QString status = curentRec.value(6).toString();
    if(status == "checking")
    {
        int reqStatus = m_dbm->getRequestStatusIndex("waiting");
        curentRec.setValue(6, reqStatus);
        m_requestModel->setRecord(m_selectedRow, curentRec);
        if(!m_requestModel->submitAll())
        {
            qDebug() << "onSendRequest:m_requestModel:submit -> SQL ERROR: " << m_requestModel->lastError().text();
            m_requestModel->revertAll();
        }
    }
}

void MainWindow::onRefreshRequest()
{
    m_requestModel->select();
}

void MainWindow::onReviewRequest()
{
    QSqlRecord curentRec = m_requestModel->record(m_selectedRow);
    ReviewDialog *reviewDialog = new ReviewDialog(curentRec, this);
    int ret = reviewDialog->exec();
    QString description = reviewDialog->getDescription();
    if(ret == 1)//accept
    {
        int reqStatus;
        if(reviewDialog->hasEditRecord())
        {
            QString type = curentRec.value(5).toString();
            QSqlRecord editRec = reviewDialog->getEditRecord();

            QByteArray data = curentRec.value("data").toByteArray();
            QList<QByteArray> datas = PacketManager::fromByteArray(data);
            QSqlRecord oldRec = PacketManager::fromByteArray(datas.at(0), m_dbm, editRec.field(0).tableName());

            addRequest(type, description, editRec, oldRec);
            reqStatus = m_dbm->getRequestStatusIndex("edited");
        }
        else
        {
            reqStatus = m_dbm->getRequestStatusIndex("accepted");
        }
        //update request status-------------------------------------
        curentRec.setValue(6, reqStatus);
        curentRec.setValue(10, description);
        m_requestModel->setRecord(m_selectedRow, curentRec);
        if(!m_requestModel->submitAll())
        {
            qDebug() << "onReviewRequest:m_requestModel:submit -> SQL ERROR: " << m_requestModel->lastError().text();
            m_requestModel->revertAll();
        }
    }
    else if(ret == -1)//reject
    {
        //update request status-------------------------------------
        int reqStatus = m_dbm->getRequestStatusIndex("rejected");
        curentRec.setValue(6, reqStatus);
        curentRec.setValue(10, description);
        m_requestModel->setRecord(m_selectedRow, curentRec);
        if(!m_requestModel->submitAll())
        {
            qDebug() << "onReviewRequest:m_requestModel:submit -> SQL ERROR: " << m_requestModel->lastError().text();
            m_requestModel->revertAll();
        }
    }
}

void MainWindow::onApplyRequest()
{
    QSqlRecord reqRec = m_requestModel->record(m_selectedRow);
    QString tableName = reqRec.value(2).toString();
    QString type = reqRec.value(5).toString();
    QString status = reqRec.value(6).toString();
    QByteArray data = reqRec.value("data").toByteArray();


    QSqlRelationalTableModel *model = m_dbm->getRelationalModelTableName(tableName);
    if(status == "accepted")
    {
        if(type == "insert")
        {
            QSqlRecord tableRec = PacketManager::fromByteArray(data, m_dbm, tableName);
            insertRow(tableRec, model);
        }
        else if(type == "delete")
        {
            QSqlRecord tableRec = PacketManager::fromByteArray(data, m_dbm, tableName);
            deleteRow(tableRec, model);
        }
        else if(type == "update")
        {
            QList<QByteArray> datas = PacketManager::fromByteArray(data);
            QSqlRecord oldRec = PacketManager::fromByteArray(datas.at(0), m_dbm, tableName);
            QSqlRecord newRec = PacketManager::fromByteArray(datas.at(1), m_dbm, tableName);
            updateRow(newRec, oldRec, model);
        }
    }
    else if(status == "rejected" || status == "edited")
    {
        if(type == "insert")
        {
            QSqlRecord tableRec = PacketManager::fromByteArray(data, m_dbm, tableName);
            deleteRow(tableRec, model);
        }
        else if(type == "delete")
        {
            QSqlRecord tableRec = PacketManager::fromByteArray(data, m_dbm, tableName);
            insertRow(tableRec, model);
        }
        else if(type == "update")
        {
            QList<QByteArray> datas = PacketManager::fromByteArray(data);
            QSqlRecord oldRec = PacketManager::fromByteArray(datas.at(0), m_dbm, tableName);
            QSqlRecord newRec = PacketManager::fromByteArray(datas.at(1), m_dbm, tableName);
            updateRow(oldRec, newRec, model);
        }
    }
    reqRec.setValue("apply", true);
    m_requestModel->setRecord(m_selectedRow, reqRec);
    if(!m_requestModel->submitAll())
    {
        qDebug() << "onApplyRequest:m_requestModel:submit -> SQL ERROR: " << m_requestModel->lastError().text();
        m_requestModel->revertAll();
    }
}

