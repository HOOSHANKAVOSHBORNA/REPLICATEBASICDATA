#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlRelationalTableModel>

#include "../common/dbmanager.h"

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
    void addInsertRequest();
    bool isRollbackable();
private slots:
    void onCustomMenuRequest(QPoint pos);
    void onAddRequest();
    void onRollbackRequest();
    void onSendRequest();
    void onRefreshRequest();
    void onReviewRequest();
private:
    Ui::MainWindow *ui;
    QSqlRelationalTableModel* m_requestModel;
    int m_selectedRow;
    DBManager *m_dbm;
};
#endif // MAINWINDOW_H
