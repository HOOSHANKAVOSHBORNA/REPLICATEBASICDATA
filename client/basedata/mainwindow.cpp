#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dbmanager.h"
#include <QDebug>
#include <QJsonDocument>
#include <QSqlRecord>
#include <QSqlField>
#include "createrequestdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomMenuRequest(QPoint)));

    DBManager *dbm  = DBManager::getDBManager();
    dbm->openConnection();

//    QSqlQueryModel* model = dbm.getRequestModel();
//    ui->tableView->setModel(model);

    requestModel = dbm->getRequestRelationalModel();
    ui->tableView->setModel(requestModel);


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
    dbm->closeConnection();
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
   /* Set the actions to the menu */
   menu->addAction(addRequest);
   menu->addAction(sendRequest);
   menu->addAction(rollbackRequest);
   /* Call the context menu */
   menu->popup(ui->tableView->viewport()->mapToGlobal(pos));
}

void MainWindow::onAddRequest()
{
    CreateRequestDialog *createRequest = new CreateRequestDialog(this);
    int ret = createRequest->exec();
    QSqlRelationalTableModel *model = createRequest->getModel();
    QList<int> insertIndexList = createRequest->getInsertIndexList();
    if(ret == QDialog::Accepted)
    {
        if(!model->submitAll())
        {
            qDebug() << "AddRequest -> SQL ERROR: " << model->lastError().text();
            return;
        }
        for(auto insertIndex:insertIndexList){
            QSqlRecord rec = model->record(insertIndex);
            QByteArray data = toByteArray(rec);
            //--------------------------------------------------
            QSqlRecord requestRec = requestModel->record();
            requestRec.remove(requestRec.indexOf("id"));
            requestRec.setValue("table_id", rec.field("id").value());
            requestRec.setValue("table_name_name_3", 1);
            requestRec.setValue("applicant", 11);
            requestRec.setValue("reviewer", 12);
            requestRec.setValue("request_type_name_2", "insert");
            requestRec.setValue("name", "checking");
            requestRec.setValue("data", data);
            requestRec.setValue("description", "{}");
            requestRec.setValue("created_at", 12345);
            qDebug() <<"requestRec:"<<requestRec;
            if(requestModel->insertRecord(-1, requestRec))
            {
                if(!requestModel->submitAll())
                {
                    qDebug() << "AddRequest requestModel -> SQL ERROR: " << requestModel->lastError().text();
                }
            }
            //------------------------------------------------
            qDebug() << "insert: " << requestModel->lastError().text();
        }
    }
    else
    {
        model->revertAll();
    }
}

