#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dbmanager.h"
#include <QDebug>
#include <QJsonDocument>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    DBManager dbm;
    dbm.openConnection();

    QSqlQueryModel* model = dbm.getRequestModel();
    ui->tableView->setModel(model);


    QList<Request> *requestList =  dbm.loadRequests();
    Request req = requestList->at(0);
    QByteArray reqData = req.toByteArray();
    Request req1;
    req1.fromByteArray(&reqData);

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
    dbm.closeConnection();
}

MainWindow::~MainWindow()
{
    delete ui;
}

