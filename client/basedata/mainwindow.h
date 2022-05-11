#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
    void addInsertRequest();
private slots:
    void onCustomMenuRequest(QPoint pos);
    void onAddRequest();
    void onRollbackRequest();
private:
    Ui::MainWindow *ui;
    QSqlRelationalTableModel* requestModel;
    int m_selectedRow;
};
#endif // MAINWINDOW_H
