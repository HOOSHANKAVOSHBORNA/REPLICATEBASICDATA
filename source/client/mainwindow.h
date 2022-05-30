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
    bool isDeleteable();
    bool isReviewable();
    bool isSendable();
    bool isApplyable();

    void insertRow(const QSqlRecord &tableRec, QSqlRelationalTableModel *model);
    void deleteRow(const QSqlRecord &tableRec, QSqlRelationalTableModel *model);
    void updateRow(const QSqlRecord &oldRec, const QSqlRecord &newRec, QSqlRelationalTableModel *model);
    void addRequest(QString typeStr, QString description, const QSqlRecord& rec, const QSqlRecord& recOld);
private slots:
    void onCustomMenuRequest(QPoint pos);
    void onAddRequest();
    void onDeleteRequest();
    void onSendRequest();
    void onRefreshRequest();
    void onReviewRequest();
    void onApplyRequest();
private:
    Ui::MainWindow *ui;
    QSqlRelationalTableModel* m_requestModel;
    int m_selectedRow;
    DBManager *m_dbm;
};
#endif // MAINWINDOW_H
