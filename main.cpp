#include <QApplication>
#include "loginwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 创建并显示登录窗口
    loginwindow loginWindow;
    loginWindow.show();

    return app.exec();
}
