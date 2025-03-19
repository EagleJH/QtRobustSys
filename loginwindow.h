#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include "mainwindow.h"
#include "registwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
namespace Ui {
class loginwindow;
}

class loginwindow : public QWidget
{
    Q_OBJECT

public:
    explicit loginwindow(QWidget *parent = nullptr);
    ~loginwindow();

private slots:
    void on_login_clicked();

    void on_regist_clicked();
    bool initializeDatabase();



private:
    Ui::loginwindow *ui;
    MainWindow *mainWindow;
    RegistWindow *regist;

};

#endif // LOGINWINDOW_H
