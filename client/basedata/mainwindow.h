#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "dbmanager.h"

#include <QMainWindow>
#include <QSqlRelationalTableModel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    QByteArray toByteArray(QSqlRecord);
    QSqlRecord fromByteArray(QByteArray, QString);
    void addInsertRequest();
private slots:
    void onCustomMenuRequest(QPoint pos);
    void onAddRequest();
    void onRollbackRequest();
    void onSendRequest();
private:
    Ui::MainWindow *ui;
    QSqlRelationalTableModel* m_requestModel;
    int m_selectedRow;
    DBManager *m_dbm;
};
#endif // MAINWINDOW_H
