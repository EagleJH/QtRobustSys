#include "registwindow.h"
#include "ui_registwindow.h"

RegistWindow::RegistWindow(QWidget *login, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegistWindow)
{

    loginPage = login;
    ui->setupUi(this);
    // 隐藏状态栏
    this->setWindowFlags(Qt::FramelessWindowHint);
    connect(ui->reminpb, &QPushButton::clicked, this, &RegistWindow::showMinimized);
    connect(ui->reclosepb, &QPushButton::clicked, this, &RegistWindow::close);
}

RegistWindow::~RegistWindow()
{

    delete ui;

}

void RegistWindow::on_pushButton_clicked()
{
    this->hide();
    loginPage->show();
}


void RegistWindow::on_registpb_clicked()
{
    QSqlQuery query;
    QString username = ui->registuser->text();
    QString password = ui->registpasswd->text();
    if(username != "" && password != ""){
        query.prepare("INSERT INTO user (username, password) VALUES (:username, :password)");
        query.bindValue(":username", username);
        query.bindValue(":password", password);

        if (!query.exec()) {
            QMessageBox::warning(this, "错误", "注册失败！");
            return;
        }
        QMessageBox::warning(this, "成功", "注册成功！");
    }else{
        QMessageBox::warning(this, "失败", "账户密码不能为空！");
    }

}
