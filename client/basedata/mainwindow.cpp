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

//    QSqlRecord requestRec = m_requestModel->record(0);s
//    qDebug() <<"requestRec:"<<requestRec;
//    qDebug()<<m_requestModel->data(m_requestModel->index(0, 2));

//    QList<Request> *requestList =  dbm.loadRequests();
//    Request req = requestList->at(0);
//    QByteArray reqData = req.toByteArray();
//    Request req1;
//    req1.fromByteArray(&reqData);

//    ui->tableWidget->setRowCount(requestList->length());
//    ui->tableWidget->setColumnCount(10);
//    ui->tableWidget->setHorizontalHeaderLabels({"id","table_id", "table_name_id",
//                                              "applicant", "reviewer", "type",
//                                             "status", "data", "description","created_at"});
//    for (int i = 0; i<requestList->length(); i++) {
//        QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg(requestList->at(i).id));
//        ui->tableWidget->setItem(i, 0, newItem);
//        QTableWidgetItem *newItem1 = new QTableWidgetItem(tr("%1").arg(requestList->at(i).table_id));
//        ui->tableWidget->setItem(i, 1, newItem1);
//        QTableWidgetItem *newItem2 = new QTableWidgetItem(tr("%1").arg(requestList->at(i).table_name_id));
//        ui->tableWidget->setItem(i, 2, newItem2);
//        QTableWidgetItem *newItem3 = new QTableWidgetItem(tr("%1").arg(requestList->at(i).applicant));
//        ui->tableWidget->setItem(i, 3, newItem3);
//        QTableWidgetItem *newItem4 = new QTableWidgetItem(tr("%1").arg(requestList->at(i).reviewer));
//        ui->tableWidget->setItem(i, 4, newItem4);
//        QTableWidgetItem *newItem5 = new QTableWidgetItem(tr("%1").arg(requestList->at(i).type));
//        ui->tableWidget->setItem(i, 5, newItem5);
//        QTableWidgetItem *newItem6 = new QTableWidgetItem(tr("%1").arg(requestList->at(i).status));
//        ui->tableWidget->setItem(i, 6, newItem6);
//        QTableWidgetItem *newItem7 = new QTableWidgetItem(QString(requestList->at(i).data));
//        ui->tableWidget->setItem(i, 7, newItem7);
//        QTableWidgetItem *newItem8 = new QTableWidgetItem(tr("%1").arg(requestList->at(i).description));
//        ui->tableWidget->setItem(i, 8, newItem8);
//        QTableWidgetItem *newItem9 = new QTableWidgetItem(tr("%1").arg(requestList->at(i).created_at));
//        ui->tableWidget->setItem(i, 9, newItem9);
////        qDebug("id:%ld", req.id);
////        qDebug("table_id:%ld",req.table_id );
////        qDebug("table_name_id:%d",req.table_name_id );
////        qDebug("applicant:%d",req.applicant );
////        qDebug("reviewer:%d",req.reviewer);
////        qDebug("type:%d",req.type );
////        qDebug("status:%d",req.status);
////        QString strData(req.data);
////        qDebug() <<"data:" << strData;
//////        QJsonDocument doc(req.description);
//////        QString strDescription(doc.toJson(QJsonDocument::Compact));
////        qDebug()<<"description:"<< req.description;
////        qDebug("created_at:%ld",req.created_at);
//    }
    //dbm->closeConnection();
}

MainWindow::~MainWindow()
{
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
   /* Connect slot handlers for Action pop-up menu */
   connect(addRequest, SIGNAL(triggered()), this, SLOT(onAddRequest()));  // Call Handler dialog editing
   connect(sendRequest, SIGNAL(triggered()), this, SLOT(onSendRequest()));
   connect(rollbackRequest, SIGNAL(triggered()), this, SLOT(onRollbackRequest()));
   /* Set the actions to the menu */
   menu->addAction(addRequest);
   m_selectedRow = ui->tableView->rowAt(pos.y());
   if(m_selectedRow != -1)
   {
       menu->addAction(sendRequest);
       menu->addAction(rollbackRequest);
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
        int applicant = 11;
        int reviewer = 12;
        qint64 time = QDateTime::currentMSecsSinceEpoch();
        for(auto insertIndex:insertIndexList)
        {
            //valid index (delete index remove from model in submitAll)
            int index = insertIndex - deleteIndexList.length();
            QSqlRecord rec = model->record(index);
            QByteArray data = toByteArray(rec);
            QSqlRecord requestRec = m_requestModel->record();
            requestRec.remove(requestRec.indexOf("id"));
            requestRec.setValue("table_id", rec.field("id").value());
            requestRec.setValue("table_name_name_3", tableIndex);
            requestRec.setValue("applicant", applicant);
            requestRec.setValue("reviewer", reviewer);
            requestRec.setValue("request_type_name_2", type);//insert
            requestRec.setValue("name", status);//checking
            requestRec.setValue("data", data);
            requestRec.setValue("description", "{}");
            requestRec.setValue("created_at", time);
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
            requestRec.setValue("table_id", delStruct.rec.field("id").value());
            requestRec.setValue("table_name_name_3", tableIndex);
            requestRec.setValue("applicant", applicant);
            requestRec.setValue("reviewer", reviewer);
            requestRec.setValue("request_type_name_2", type);//delete
            requestRec.setValue("name", status);//checking
            requestRec.setValue("data", data);
            requestRec.setValue("description", "{}");
            requestRec.setValue("created_at", time);
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

void MainWindow::onRollbackRequest()
{
    QSqlRecord reqRec = m_requestModel->record(m_selectedRow);
    QString tableName = reqRec.value("table_name_name_3").toString();
    QString type = reqRec.value("request_type_name_2").toString();
    QByteArray data = reqRec.value("data").toByteArray();
    QSqlRecord tableRec = fromByteArray(data, tableName);
//    qDebug()<< reqRec;
//    qDebug()<< tableRec;
    QSqlRelationalTableModel *model = m_dbm->getRelationalModelTableName(tableName);
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
    QSqlRecord rec = model->record();
    rec.remove(rec.indexOf("id"));
    rec.setValue("request_id",reqRec.value("id"));
    rec.setValue("receiver",2);
    int ackStatus = m_dbm->getAckStatusIndex("pending");
    rec.setValue("status",ackStatus);
    if(model->insertRecord(-1, rec))
    {
        if(!model->submitAll())
        {
            qDebug() << "onSendRequest:model:insert ack -> SQL ERROR: " << model->lastError().text();
            return;
        }
    }
    //update request status-------------------------------------
    int reqStatus = m_dbm->getRequestStatusIndex("waiting");
    QSqlRecord curentRec = m_requestModel->record(m_selectedRow);
    curentRec.setValue("name", reqStatus);
    m_requestModel->setRecord(m_selectedRow, curentRec);
    if(!m_requestModel->submitAll())
    {
        qDebug() << "onSendRequest:m_requestModel:submit -> SQL ERROR: " << m_requestModel->lastError().text();
        m_requestModel->revertAll();
    }
}

