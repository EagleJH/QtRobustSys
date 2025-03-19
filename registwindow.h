#ifndef REGISTWINDOW_H
#define REGISTWINDOW_H
#include <QDebug>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QWidget>

namespace Ui {
class RegistWindow;
}

class RegistWindow : public QWidget
{
    Q_OBJECT

public:
    explicit RegistWindow(QWidget *login, QWidget *parent = nullptr);
    ~RegistWindow();

private slots:
    void on_pushButton_clicked();

    void on_registpb_clicked();

private:
    Ui::RegistWindow *ui;
    QWidget *loginPage;
};

#endif // REGISTWINDOW_H
