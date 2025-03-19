#include "loginwindow.h"
#include "ui_loginwindow.h"


loginwindow::loginwindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::loginwindow)
{

    ui->setupUi(this);
    if (!initializeDatabase()) {
        QMessageBox::critical(this, "错误", "无法初始化数据库！");
    }
    // 隐藏状态栏
    this->setWindowFlags(Qt::FramelessWindowHint);
    connect(ui->loginminpb, &QPushButton::clicked, this, &loginwindow::showMinimized);
    connect(ui->loginclosepb, &QPushButton::clicked, this, &loginwindow::close);

}

loginwindow::~loginwindow()
{

    delete ui;

}

void loginwindow::on_login_clicked()
{
    QString username = ui->user->text();
    QString password = ui->passwd->text();
    QSqlQuery query;
    query.prepare("SELECT * FROM user WHERE username = :username AND password = :password");
    query.bindValue(":username", username);
    query.bindValue(":password", password);

    if (!query.exec()) {
        qDebug() << "查询失败:" << query.lastError().text();

    }

    if (query.next()) {
        qDebug() << "登录成功！";
        // 登录成功
        this->hide();  // 隐藏登录窗口
        mainWindow = new MainWindow();
        mainWindow->show();

    } else {
        QMessageBox::warning(this, "错误", "账户或密码错误！");

    }

}


void loginwindow::on_regist_clicked()
{

    this->hide();
    regist = new RegistWindow(this);
    regist->show();
}

bool loginwindow::initializeDatabase() {
    // 创建 SQLite 数据库连接
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("userdb.db"); // 数据库文件名

    if (!db.open()) {
        qDebug() << "无法打开数据库:" << db.lastError().text();
        return false;
    }

    // 创建 user 表（如果不存在）
    QSqlQuery query;
    QString createTableQuery = R"(
        CREATE TABLE IF NOT EXISTS user (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT NOT NULL UNIQUE,
            password TEXT NOT NULL
        )
    )";

    if (!query.exec(createTableQuery)) {
        qDebug() << "创建表失败:" << query.lastError().text();
        return false;
    }

    qDebug() << "数据库初始化成功！";
    return true;
}


