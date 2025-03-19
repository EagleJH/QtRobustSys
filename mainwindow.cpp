#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // 隐藏状态栏
    this->setWindowFlags(Qt::FramelessWindowHint);
    process = new QProcess(this);
    process2 = new QProcess(this);

    ui->setupUi(this);
    connect(ui->pb1, &QPushButton::clicked, this, [=]() {tabswitch(0);});
    connect(ui->pb2, &QPushButton::clicked, this, [=]() {tabswitch(1);});
    connect(ui->pb3, &QPushButton::clicked, this, [=]() {tabswitch(2);});
    connect(ui->pb4, &QPushButton::clicked, this, [=]() {tabswitch(3);});
    connect(ui->pb5, &QPushButton::clicked, this, [=]() {tabswitch(4);});
    connect(ui->pb6, &QPushButton::clicked, this, [=]() {tabswitch(5);});
    connect(ui->minpb, &QPushButton::clicked, this, &MainWindow::showMinimized);
    connect(ui->closepb, &QPushButton::clicked, this, &MainWindow::close);

    connect(ui->npydata, &QPushButton::clicked, this, [=]() {dataswitch(0);});
    connect(ui->pngdata, &QPushButton::clicked, this, [=]() {dataswitch(1);});

    connect(ui->runpb, &QPushButton::clicked, this, [=]() {startProcess();});
    connect(ui->genpb, &QPushButton::clicked, this, [=]() {generateProcess();});

    // 绑定槽函数
    connect(process, &QProcess::readyReadStandardOutput, this, [this]() {
        readOutput(ui->textEdit);  // 传入 outputTextEdit
    });

    connect(process, &QProcess::readyReadStandardError, this, [this]() {
        readError(ui->textEdit);    // 传入 errorTextEdit
    });

    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [this](int exitCode, QProcess::ExitStatus exitStatus) {
        processFinished(exitCode, exitStatus, ui->textEdit);  // 传入 outputTextEdit
    });

    // 绑定槽函数
    connect(process2, &QProcess::readyReadStandardOutput, this, [this]() {
        readOutput(ui->detectlog);  // 传入 outputTextEdit
    });

    connect(process2, &QProcess::readyReadStandardError, this, [this]() {
        readError(ui->detectlog);    // 传入 errorTextEdit
    });

    connect(process2, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [this](int exitCode, QProcess::ExitStatus exitStatus) {
        processFinished(exitCode, exitStatus, ui->detectlog);  // 传入 outputTextEdit
    });




    ui->stackedWidget->setCurrentIndex(0);
    ui->datacombox->setCurrentIndex(0);
    ui->classes->setText("2");
    ui->classes_2->setText("2");
    ui->npyrun->setVisible(false);
}


MainWindow::~MainWindow()
{
    delete ui;
    if (process->state() == QProcess::Running) {
            process->terminate();  // 确保进程终止
            process->waitForFinished();
    }
}


void MainWindow::initnpyTable(QTableWidget *table){
    if (ui->npytable->horizontalHeader()->count() == 0) {
        // 设置水平表头
        qDebug()<<"初始化表头";
        table->setColumnCount(128);  // 设置列数为 128
        QStringList horizontalHeaders;
        for (int i = 0; i < 128; ++i) {
            horizontalHeaders << QString("L%1").arg(i);  // 生成列名 L0, L1, ..., L127
        }
        table->setHorizontalHeaderLabels(horizontalHeaders);
    }

}
void MainWindow::tabswitch(int num){

    ui->stackedWidget->setCurrentIndex(num);
    if(num == 4 ){
        initnpyTable(ui->npytable);
    }else if(num == 1){
        initnpyTable(ui->advtable);
    }else if(num == 3){
        initDetectTable();
    }else if (num == 5) {
        ui->loss_re->setVisible(true);
        ui->matrix->setVisible(false);
        QList<QString> names;
        QList<QPointF> data = getRecentData(names);
        drawLine(data, names, ui->advView);

        QVector<double> datalast = getLastData();
        drawBar(datalast, ui->barView);
        ui->stackedWidgetRe->setCurrentIndex(0);
    }
}

void MainWindow::on_malwarepb_clicked()
{
    // 打开文件选择对话框
       QString filePath = QFileDialog::getOpenFileName(
           this,                               // 父窗口
           tr("选择文件"),                     // 对话框标题
           QDir::homePath(),                   // 默认打开路径（用户主目录）
           tr("所有文件 (*);;文本文件 (*.txt)") // 文件过滤器
       );

       // 如果用户选择了文件
       if (!filePath.isEmpty())
       {
           // 将文件路径设置到 QLineEdit 中
           ui->malwaretxt->setText(filePath);
       }
}


void MainWindow::on_benignpb_clicked()
{
    // 打开文件选择对话框
       QString filePath = QFileDialog::getOpenFileName(
           this,                               // 父窗口
           tr("选择文件"),                     // 对话框标题
           QDir::homePath(),                   // 默认打开路径（用户主目录）
           tr("所有文件 (*);;文本文件 (*.txt)") // 文件过滤器
       );

       // 如果用户选择了文件
       if (!filePath.isEmpty())
       {
           // 将文件路径设置到 QLineEdit 中
           ui->benigntxt->setText(filePath);
       }
}


void MainWindow::on_pthpb_clicked()
{
    // 打开文件选择对话框
       QString filePath = QFileDialog::getOpenFileName(
           this,                               // 父窗口
           tr("选择文件"),                     // 对话框标题
           QDir::homePath(),                   // 默认打开路径（用户主目录）
           tr("所有文件 (*);;文本文件 (*.txt)") // 文件过滤器
       );

       // 如果用户选择了文件
       if (!filePath.isEmpty())
       {
           // 将文件路径设置到 QLineEdit 中
           ui->pthtxt->setText(filePath);
       }
}


void MainWindow::on_flashpb_clicked()
{
    ui->vectortxt->clear();
    ui->batchtxt->clear();
    ui->epochtxt->clear();
    ui->malwaretxt->clear();
    ui->benigntxt->clear();
    ui->pthtxt->clear();
    ui->textEdit->clear();
}

void MainWindow::drawBar(QVector<double> &p, QChartView *view){
    // 创建柱状图的数据集
        QtCharts::QBarSet *set0 = new QtCharts::QBarSet("Origin");
        QtCharts::QBarSet *set1 = new QtCharts::QBarSet("Adv.Gan");
        QtCharts::QBarSet *set2 = new QtCharts::QBarSet("Benign");

        // 添加数据
        *set0 << p[0]; // Origin 的值
        *set1 << p[1]; // Adv.Gan 的值
        *set2 << p[2]; // Benign 的值

        // 创建柱状图系列
        QtCharts::QBarSeries *series = new QtCharts::QBarSeries();
        series->append(set0);
        series->append(set1);
        series->append(set2);

        // 创建图表
        QtCharts::QChart *chart = new QtCharts::QChart();
        chart->addSeries(series); // 将系列添加到图表
        chart->setAnimationOptions(QtCharts::QChart::SeriesAnimations); // 启用动画

        // 创建 X 轴
        QtCharts::QBarCategoryAxis *axisX = new QtCharts::QBarCategoryAxis();
        axisX->append("结果对比"); // X 轴的标签
        chart->addAxis(axisX, Qt::AlignBottom); // 将 X 轴添加到图表底部
        series->attachAxis(axisX); // 将系列与 X 轴关联

        // 创建 Y 轴
        QtCharts::QValueAxis *axisY = new QtCharts::QValueAxis();
        axisY->setRange(0, 1); // 设置 Y 轴范围
        chart->addAxis(axisY, Qt::AlignLeft); // 将 Y 轴添加到图表左侧
        series->attachAxis(axisY); // 将系列与 Y 轴关联

        view->setChart(chart);

}

void MainWindow::drawLine(QList<QPointF> &points, QList<QString> names, QChartView *view){
    QChart *chart = new QChart();

    // 创建折线序列
    QLineSeries *series1 = new QLineSeries();
    QLineSeries *series2 = new QLineSeries();
    for (int i=0;i<7;i++) {
        series1->append(points[i].x()-1, points[i].y());
        series2->append(points[i+7].x()-1, points[i+7].y());
    }


    series1->setName("Origin");
    series2->setName("Adv.Gen");

    // 将序列添加到图表
    chart->addSeries(series1);
    chart->addSeries(series2);
    chart->setAnimationOptions(QtCharts::QChart::SeriesAnimations); // 启用动画
    // 创建坐标轴
    QCategoryAxis *x = new QCategoryAxis();
    QValueAxis *y = new QValueAxis();
    y->setRange(0, 1);
    y->setLabelFormat("%0.2f");

//    x->setRange(0, 6); // 设置X轴范围
    for (int i=0;i<7;i++) {
        x->append(QString("%1:%2").arg(i+1).arg(names[i]), i+1);
    }

    chart->setAxisY(y, series1);
    chart->setAxisY(y, series2);
    chart->setAxisX(x, series1);
    chart->setAxisX(x, series2);

    // 创建图表视图
    view->setChart(chart);
}

void MainWindow::drawSingleLine(QString csvpath, QChartView *view){
    QChart *chart = new QChart();


    // 打开 CSV 文件
    QFile file(csvpath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "无法打开文件！";
    }

    // 读取文件内容
    QTextStream in(&file);
    QStringList lines;
    while (!in.atEnd()) {
        lines.append(in.readLine()); // 逐行读取
    }
    file.close();

    // 创建折线序列
    int size = lines.size();
    QSplineSeries *series = new QSplineSeries();
    QValueAxis *x = new QValueAxis();
    x->setRange(0, size);
    series->setName("loss curve");
    QValueAxis *y = new QValueAxis();
    y->setRange(0, 1.8);

    for (int i = 0; i < size; ++i) {
        QString line = lines[i];
        QStringList columns = line.split(','); // 按逗号分割
        series->append(i, columns.last().toDouble());
    }
    chart->addSeries(series);
    chart->setAnimationOptions(QtCharts::QChart::SeriesAnimations); // 启用动画
     // 创建图表视图
    view->setChart(chart);
}

QVector<double> MainWindow::getLastData(){
    QFile file("D:/python_proj/gan/results.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "无法打开文件！";
    }
    // 读取文件内容
    QTextStream in(&file);
    QStringList lines;
    while (!in.atEnd()) {
        lines.append(in.readLine()); // 逐行读取
    }
    file.close();
    // 提取最后一行

    QStringList lastSevenLines = lines.mid(lines.size() - 1);
    QVector<double> data;
    QStringList column = lastSevenLines[0].split(',');
    // 检查列数是否足够
    if (column.size() < 14) {
        qWarning() <<"行列数不足！";
        return data;
    }
    data.append(column[14].toDouble());
    data.append(column[15].toDouble());
    data.append(column[16].toDouble());
    return data;
}

QList<QPointF> MainWindow::getRecentData(QList<QString> &names){
    // 打开 CSV 文件
     QFile file("D:/python_proj/gan/results.csv");
     if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
         qWarning() << "无法打开文件！";
     }

     // 读取文件内容
     QTextStream in(&file);
     QStringList lines;
     while (!in.atEnd()) {
         lines.append(in.readLine()); // 逐行读取
     }
     file.close();

     // 检查文件是否至少有七行
     if (lines.size() < 7) {
         qWarning() << "文件行数不足七行！";
     }

     // 提取最后七行
     QStringList lastSevenLines = lines.mid(lines.size() - 7);

     // 存储第十三列和第十四列数据
     QList<QPointF> column14Points; // 第一组 QPoint
     QList<QPointF> column15Points; // 第二组 QPoint

     for (int i = 0; i < lastSevenLines.size(); ++i) {
         QString line = lastSevenLines[i];
         QStringList columns = line.split(','); // 按逗号分割

         // 检查列数是否足够
         if (columns.size() < 14) {
             qWarning() << "第" << i + 1 << "行列数不足！";
             continue;
         }

         // 提取第十三列和第十四列数据
         bool ok1, ok2;
         double column14 = columns[14].toDouble(&ok1); // 第十三列（索引 12）
         double column15 = columns[15].toDouble(&ok2); // 第十四列（索引 13）

         if (!ok1 || !ok2) {
             qWarning() << "第" << i + 1 << "行数据格式错误！";
             continue;
         }

         // 存储到 QList<QPoint>
         column14Points.append(QPointF(i + 1, column14)); // x 为 1~7，y 为第十四列数据
         column15Points.append(QPointF(i + 1, column15)); // x 为 1~7，y 为第十五列数据
         names.append(columns[5]);
     }

//     // 输出结果
//     qDebug() << "第一组 QPoint（第十三列数据）：";
//     for (const QPointF &point : column14Points) {
//         qDebug() << "x:" << point.x() << "y:" << point.y();
//     }

//     qDebug() << "第二组 QPoint（第十四列数据）：";
//     for (const QPointF &point : column15Points) {
//         qDebug() << "x:" << point.x() << "y:" << point.y();
//     }
     column14Points.append(column15Points);
     return column14Points;

}

void MainWindow::dataswitch(int num){
    ui->dataStack->setCurrentIndex(num);
    if(num == 1){
        ui->labeldex->clear();
        ui->labelpic->clear();
    }
}

void MainWindow::startProcess(QString gen) {
    // 清空输出
    ui->textEdit->clear();
    // 获取 QLineEdit 中的文本
    QString vector = ui->vectortxt->text();
    QString batch = ui->batchtxt->text();
    QString epoch = ui->epochtxt->text();
    QString malware = ui->malwaretxt->text();
    QString benign = ui->benigntxt->text();
    QString detector = ui->classfiertxt->currentText();

    QDateTime currentDateTime = QDateTime::currentDateTime();

    // 格式化时间为 "年-月-日-时-分-秒"
    QString formattedTime = currentDateTime.toString("yyyy-MM-dd-HH-mm-ss");

    QString type = "train";
    QString command;
    QString pth = ui->pthtxt->text();
    if(pth != ""){
        type = "load";
    }
    command = QString("conda activate padenv && python D:/python_proj/gan/main.py %1 %2 %3 %4 %5 --detector %6 --type %7 --pth %8 --gen %9 --time %10")
                              .arg(vector)
                              .arg(batch)
                              .arg(epoch)
                              .arg(malware)
                              .arg(benign)
                              .arg(detector)
                              .arg(type)
                              .arg(pth==""?"test":pth)
                              .arg(gen)
                              .arg(formattedTime);


    // 打印命令（调试用）
    qDebug() << "Running command:" << command;


    // 启动进程
    process->start("cmd.exe", QStringList() << "/c" << command);

    // 检查是否启动成功
    if (!process->waitForStarted()) {
        QMessageBox::critical(this, "错误", "无法启动 Python 脚本！");
        return;
    }
}

void MainWindow::generateProcess(QString gen) {
    // 清空输出
    ui->advtable->setRowCount(0);
    // 获取 QLineEdit 中的文本
    QString noise = ui->noise->text();
    QString count = ui->count->text();
    QString epoch = "1";
    QString malware = ui->malware->text();
    QString benign = ui->benign->text();
    QString detector = ui->detector->currentText();
    QString params = ui->params->text();
    QDateTime currentDateTime = QDateTime::currentDateTime();

    // 格式化时间为 "年-月-日-时-分-秒"
    QString formattedTime = currentDateTime.toString("yyyy-MM-dd-HH-mm-ss");

    QString type = "load";
    QString command;

    command = QString("conda activate padenv && python D:/python_proj/gan/main.py %1 %2 %3 %4 %5 --detector %6 --type %7 --pth %8 --gen %9 --time %10")
                              .arg(noise)
                              .arg(count)
                              .arg(epoch)
                              .arg(malware)
                              .arg(benign)
                              .arg(detector)
                              .arg(type)
                              .arg(params)
                              .arg(gen)
                              .arg(formattedTime);


    // 打印命令（调试用）
    qDebug() << "Running command:" << command;


    // 启动进程
    process->start("cmd.exe", QStringList() << "/c" << command);

    // 检查是否启动成功
    if (!process->waitForStarted()) {
        QMessageBox::critical(this, "错误", "无法启动 Python 脚本！");
        return;
    }
    if(process->waitForFinished()){

        // 将csv文件图形化显示
        QString csvPath = QString("D:/python_proj/gan/generate/generate_%1.csv").arg(formattedTime);
        qDebug()<<"csvpath:"<<csvPath;
        loadCsvFile(csvPath, ui->advtable);
    }
}


void MainWindow::readOutput(QTextEdit *text) {
    QProcess* process = qobject_cast<QProcess*>(sender());  // 获取发送信号的 QProcess 对象
    if (process) {
        QByteArray output = process->readAllStandardOutput();  // 读取标准输出
        text->append(QString::fromLocal8Bit(output));      // 将输出追加到 QTextEdit
    }

}

void MainWindow::readError(QTextEdit *text) {
    QProcess* process = qobject_cast<QProcess*>(sender());  // 获取发送信号的 QProcess 对象
    if (process) {
       QByteArray error = process->readAllStandardError();  // 读取错误输出
       text->append(QString::fromLocal8Bit(error));     // 将错误输出追加到 QTextEdit
    }
}

void MainWindow::processFinished(int exitCode, QProcess::ExitStatus exitStatus, QTextEdit *text) {
    QProcess* process = qobject_cast<QProcess*>(sender());  // 获取发送信号的 QProcess 对象
    if (process) {
        QString message;
        if (exitStatus == QProcess::NormalExit) {
            message = QString("进程正常结束，退出代码：%1").arg(exitCode);
        } else {
            message = QString("进程异常结束，退出代码：%1").arg(exitCode);
        }
        text->append(message);  // 将结束信息追加到 QTextEdit
    }
}

void MainWindow::npyToCsv(QString path){
    if(path != ""){
        // 转换二进制文件为csv文件
        // 获取当前时间
        QDateTime currentDateTime = QDateTime::currentDateTime();

        // 格式化时间为 "年-月-日-时-分-秒"
        QString formattedTime = currentDateTime.toString("yyyy-MM-dd-HH-mm-ss");
        QString savepath = "D:/Users/Administrator/Desktop/svg/prj";

        QString command = QString("python D:\\Users\\Administrator\\Desktop\\svg\\prj\\transfer.py %1 %2 %3").arg(path).arg(formattedTime).arg(savepath);
         // 启动进程
        process->start("cmd.exe", QStringList() << "/c" << command);
         // 将csv文件进行图形化展示
         // 等待脚本执行完成
        if(process->waitForFinished()){


            QString csvFilePath = QString(savepath+"/data_%1.csv").arg(formattedTime);
            qDebug()<<csvFilePath;
            if (csvFilePath.isEmpty()) {
                QMessageBox::critical(this, "错误", "未生成 CSV 文件！");
                return;
            }

             // 加载 CSV 文件到 QTableWidget
            loadCsvFile(csvFilePath, ui->npytable);
        }
    }
}

void MainWindow::on_npyrun_clicked(){
    QString path = ui->path->text();

    npyToCsv(path);

}

void MainWindow::loadCsvFile(QString csvPath, QTableWidget *table){
    QFile file(csvPath);
    if (!file.open(QIODevice::ReadOnly| QIODevice::Text)) {
        QMessageBox::critical(this, "错误", "无法打开 CSV 文件！");
        return;
    }
    // 清空表格
    table->setRowCount(0);

    // 读取 CSV 文件
    QTextStream in(&file);
    int row = 0;
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(',');

        // 设置表格行数和列数
        if (row == 0) {
            table->setColumnCount(fields.size());
        }
        table->setRowCount(row + 1);

        // 填充表格
        for (int col = 0; col < fields.size(); ++col) {
            table->setItem(row, col, new QTableWidgetItem(fields[col]));
        }
        row++;
    }

    file.close();
}


void MainWindow::on_apkrun_clicked()
{
    if(ui->path->text() != ""){

        QString apkpath, baksmali, npyname;
        apkpath = ui->path->text();
        QFileInfo fileInfo(apkpath);
        QString apkname = fileInfo.baseName();  // 获取文件名（不带后缀）

        baksmali = "D:/Users/Administrator/Desktop/svg/prj/baksmali-2.5.2.jar";
        QDateTime currentDateTime = QDateTime::currentDateTime();
        QString formattedTime = currentDateTime.toString("yyyy-MM-dd-HH-mm-ss");
        npyname  = QString("D:/Users/Administrator/Desktop/svg/prj/%1.npy").arg(apkname+formattedTime);

        QString command = QString("python D:/Users/Administrator/Desktop/svg/prj/apktools.py %1 %2 %3").arg(apkpath).arg(baksmali).arg(npyname);
        process->start("cmd.exe", QStringList() << "/c" << command);
        // 等待脚本执行完成
       if(process->waitForFinished()){

           npyToCsv(npyname);
           QMessageBox::warning(this, "成功", "apk已成功转换为npy文件！");
       }

    }



}


void MainWindow::on_datacombox_activated(int index)
{
    QString filePath;
    if(index == 0){
        ui->apkrun->setVisible(true);
        ui->npyrun->setVisible(false);
        filePath = QFileDialog::getOpenFileName(
               this,                               // 父窗口
               tr("选择文件"),                     // 对话框标题
               QDir::homePath(),                   // 默认打开路径（用户主目录）
               tr("APK文件 (*.apk)") // 文件过滤器
        );
    }else if(index == 1){
        ui->apkrun->setVisible(false);
        ui->npyrun->setVisible(true);
        filePath = QFileDialog::getOpenFileName(
               this,                               // 父窗口
               tr("选择文件"),                     // 对话框标题
               QDir::homePath(),                   // 默认打开路径（用户主目录）
               tr("npy文件 (*.npy)") // 文件过滤器
        );
    }
   // 如果用户选择了文件
   if (!filePath.isEmpty())
   {
       // 将文件路径设置到 QLineEdit 中
       ui->path->setText(filePath);
   }
}

void MainWindow::on_apkchs_clicked()
{
    QString filePath;
    filePath = QFileDialog::getOpenFileName(
           this,                               // 父窗口
           tr("选择文件"),                     // 对话框标题
           QDir::homePath(),                   // 默认打开路径（用户主目录）
           tr("APK文件 (*.apk)") // 文件过滤器
    );
    // 如果用户选择了文件
    if (!filePath.isEmpty())
    {
        // 将文件路径设置到 QLineEdit 中
        ui->path2->setText(filePath);
    }

}


void MainWindow::on_apktrans_clicked()
{
    if(ui->path2->text() != ""){
        // 转换为特征图
        QString apkpath = ui->path2->text();

        QFileInfo fileInfo(apkpath);
        QString apkname = fileInfo.baseName();  // 获取文件名（不带后缀）

        QDateTime currentDateTime = QDateTime::currentDateTime();
        QString formattedTime = currentDateTime.toString("yyyy-MM-dd-HH-mm-ss");
        QString picName = apkname+formattedTime+".png";
        QString command = QString("conda activate padenv && python D:/Users/Administrator/Desktop/svg/prj/create_images.py %1 %2").arg(apkpath).arg(formattedTime);
        process->start("cmd.exe", QStringList() << "/c" << command);

        if(process->waitForFinished()){
            qDebug()<<picName;
            //显示照片
            QPixmap pixmap("D:/Users/Administrator/Desktop/svg/prj/binary-image.png");
            QPixmap scaledPixmap = pixmap.scaled(300, 300, Qt::KeepAspectRatio);
            ui->labeldex->setPixmap(scaledPixmap);
            ui->labelpic->setPixmap(QPixmap("D:/Users/Administrator/Desktop/svg/prj/dex/"+picName));
        }
    }


}


void MainWindow::on_loss_re_clicked()
{
    QString filePath;
    filePath = QFileDialog::getOpenFileName(
           this,                               // 父窗口
           tr("选择文件"),                     // 对话框标题
           QDir::homePath(),                   // 默认打开路径（用户主目录）
           tr("APK文件 (*.csv)") // 文件过滤器
    );
    // 如果用户选择了文件
    if (!filePath.isEmpty())
    {
        // 将文件路径设置到 QLineEdit 中
        drawSingleLine(filePath, ui->lossView);
    }

}

void MainWindow::on_advpb1_clicked()
{
    QString filePath;
    filePath = QFileDialog::getOpenFileName(
           this,                               // 父窗口
           tr("选择文件"),                     // 对话框标题
           QDir::homePath(),                   // 默认打开路径（用户主目录）
           tr("模型参数文件 (*.pth)") // 文件过滤器
    );
    // 如果用户选择了文件
    ui->params->setText(filePath);
}


void MainWindow::on_advpb2_clicked()
{
    QString filePath;
    filePath = QFileDialog::getOpenFileName(
           this,                               // 父窗口
           tr("选择文件"),                     // 对话框标题
           QDir::homePath(),                   // 默认打开路径（用户主目录）
           tr("npy文件 (*.npy)") // 文件过滤器
    );
    // 如果用户选择了文件
    ui->malware->setText(filePath);
}


void MainWindow::on_advpb3_clicked()
{
    QString filePath;
    filePath = QFileDialog::getOpenFileName(
           this,                               // 父窗口
           tr("选择文件"),                     // 对话框标题
           QDir::homePath(),                   // 默认打开路径（用户主目录）
           tr("npy文件 (*.npy)") // 文件过滤器
    );
    // 如果用户选择了文件
    ui->benign->setText(filePath);
}



void MainWindow::on_clearpb_clicked()
{
    ui->detectlog->clear();
    ui->lr->clear();
    ui->step->clear();
    ui->gamma->clear();
    ui->experiment->setCurrentIndex(0);
    ui->classes->setText("2");
    ui->datapath->clear();
    ui->modelpath->clear();
}

void MainWindow::on_modelpb_clicked()
{
    QString filePath;
    filePath = QFileDialog::getOpenFileName(
           this,                               // 父窗口
           tr("选择文件"),                     // 对话框标题
           QDir::homePath(),                   // 默认打开路径（用户主目录）
           tr("pth文件 (*.pth)") // 文件过滤器
    );
    // 如果用户选择了文件
    ui->modelpath->setText(filePath);
}

void MainWindow::on_datapb_clicked()
{
    QString folderPath;
    folderPath = QFileDialog::getExistingDirectory(
        this,                               // 父窗口
        tr("选择文件夹"),                   // 对话框标题
        QDir::homePath()                    // 默认打开路径（用户主目录）
    );

    // 如果用户选择了文件夹
    if (!folderPath.isEmpty()) {
        ui->datapath->setText(folderPath);  // 将文件夹路径设置到 UI 中
    } else {
        QMessageBox::warning(this, "警告", "未选择文件夹！");
    }
}

void MainWindow::on_experiment_activated(int index)
{
    if(index == 0){
        ui->classes->setText("2");
    }else{
        ui->classes->setText("696");
    }
}

void MainWindow::on_detectpb_clicked()
{
    ui->detectlog->clear();
    // 获取 QLineEdit 中的文本
    double lr = ui->lr->text().toDouble();
    QString step = ui->step->text();
    QString gamma = ui->gamma->text();;
    QString experiment = ui->experiment->currentText();
    QString classes = ui->classes->text();
    QString datapath = ui->datapath->text();
    QString modelpath = ui->modelpath->text();
    QString epoch = ui->epoch->text();
    QString batchsize = ui->batchsize->text();

//    QDateTime currentDateTime = QDateTime::currentDateTime();
//    // 格式化时间为 "年-月-日-时-分-秒"
//    QString formattedTime = currentDateTime.toString("yyyy-MM-dd-HH-mm-ss");

    QString mode = "train";
    QString command;
    QString pyfile = "D:/Users/Administrator/Desktop/paper_with_code/code/Sherlock/regenerate_experiment_results.py";

    command = QString("conda activate padenv && python %1 --model_path %2 --nb_classes %3 --data_path %4 --experiment %5 --mode %6 --lr %7 --step %8 --gamma %9 --epoch %10 --batch_size %11")
                              .arg(pyfile)
                              .arg(modelpath)
                              .arg(classes)
                              .arg(datapath)
                              .arg(experiment)
                              .arg(mode)
                              .arg(lr)
                              .arg(step)
                              .arg(gamma)
                              .arg(epoch)
                              .arg(batchsize);


    // 打印命令（调试用）
    qDebug() << "Running command:" << command;


    // 启动进程
    process2->start("cmd.exe", QStringList() << "/c" << command);

    // 检查是否启动成功
    if (!process2->waitForStarted()) {
        QMessageBox::critical(this, "错误", "无法启动 Python 脚本！");
        return;
    }
    // 连接 finished 信号到槽函数
    connect(process2, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [this](int exitCode, QProcess::ExitStatus exitStatus) {
        qDebug() << "Python 脚本执行完成！";
        if (exitStatus == QProcess::NormalExit) {
            qDebug() << "退出代码：" << exitCode;
            // 将 CSV 文件图形化显示
//            visualizeCSV();
        } else {
            QMessageBox::critical(this, "错误", "Python 脚本异常结束！");
        }
    });
}


void MainWindow::on_stoppb_clicked()
{
    if (process2->state() == QProcess::Running) {
            process2->terminate();  // 尝试正常终止进程
            if (!process2->waitForFinished(1000)) {  // 等待 1 秒
                process2->kill();  // 如果未终止，强制终止进程
            }
            qDebug() << "Python 脚本已停止！";
        } else {
            qDebug() << "没有正在运行的 Python 脚本！";
        }
}

void MainWindow::on_experiment_2_activated(int index)
{
    if(index == 0){
        ui->classes_2->setText("2");
    }else{
        ui->classes_2->setText("696");
    }
}

void MainWindow::on_detectpb_2_clicked()
{
    ui->detectable->setRowCount(0);
    // 获取 QLineEdit 中的文本

    QString experiment = ui->experiment_2->currentText();
    QString classes = ui->classes_2->text();
    QString datapath = ui->datapath_2->text();
    QString modelpath = ui->modelpath_2->text();


    QDateTime currentDateTime = QDateTime::currentDateTime();
    // 格式化时间为 "年-月-日-时-分-秒"
    QString formattedTime = currentDateTime.toString("yyyy-MM-dd-HH-mm-ss");
    QString resultdir = "D:/Users/Administrator/Desktop/paper_with_code/code/Sherlock/evaluation";
    QString result = QString("%1/predic_probabilities_%2_%3.csv").arg(resultdir).arg(experiment).arg(formattedTime);


    QString command;
    QString pyfile = "D:/Users/Administrator/Desktop/paper_with_code/code/Sherlock/model_test.py";

    command = QString("conda activate padenv && python %1 --model_path %2 --nb_classes %3 --data_path %4 --experiment %5 --time %6")
                              .arg(pyfile)
                              .arg(modelpath)
                              .arg(classes)
                              .arg(datapath)
                              .arg(experiment)
                              .arg(formattedTime);


    // 打印命令（调试用）
    qDebug() << "Running command:" << command;


    // 启动进程
    process2->start("cmd.exe", QStringList() << "/c" << command);

    // 检查是否启动成功
    if (!process2->waitForStarted()) {
        QMessageBox::critical(this, "错误", "无法启动 Python 脚本！");
        return;
    }

    // 连接信号槽，监听进程完成事件
    connect(process2, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
           this, [this, result](int exitCode, QProcess::ExitStatus exitStatus) {
       if (exitStatus == QProcess::NormalExit && exitCode == 0) {
           // Python 脚本执行成功，显示结果
           showDetectResult(result);
       } else {
           // Python 脚本执行失败
           QMessageBox::critical(this, "错误", "Python 脚本执行失败！");
       }
    });


}
void MainWindow::showDetectResult(QString path){
    // 打开 CSV 文件
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "错误", "无法打开 CSV 文件！");
        return;
    }

    QTextStream in(&file);
    int row = 0;

    // 读取 CSV 文件
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(',');

        // 跳过第一行（表头）
        if (row == 0) {
            row++;
            continue;
        }

        // 设置表格行数
        ui->detectable->setRowCount(row);

        // 填充表格数据
        for (int col = 1; col <= 3; ++col) {  // 读取第 2、3、4 列
            QTableWidgetItem *item = new QTableWidgetItem(fields[col]);
            ui->detectable->setItem(row - 1, col - 1, item);  // 行索引从 0 开始
        }

        row++;
    }

    file.close();

}

void MainWindow::initDetectTable(){
    if (ui->detectable->horizontalHeader()->count() == 0) {
        ui->detectable->setColumnCount(3);;
        qDebug()<<"初始化检测表格";
        // 设置水平表头
        // 设置表头
        QStringList headers = {"Actual", "Predict", "Name"};
        ui->detectable->setHorizontalHeaderLabels(headers);
        // 设置表头的列拉伸模式，使各列铺满整个表格
        ui->detectable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    }
}

void MainWindow::on_stoppb_2_clicked()
{
    /*String path = "D:/Users/Administrator/Desktop/paper_with_code/code/Sherlock/evaluation/predic_probabilities_binary_2025-03-05 16-21-51.csv";
    showDetectResult(path);*/
    if (process2->state() == QProcess::Running) {
            process2->terminate();  // 尝试正常终止进程
            if (!process2->waitForFinished(1000)) {  // 等待 1 秒
                process2->kill();  // 如果未终止，强制终止进程
            }
            qDebug() << "Python 脚本已停止！";
        } else {
            qDebug() << "没有正在运行的 Python 脚本！";
        }
}

void MainWindow::on_clearpb_2_clicked()
{
    ui->detectable->setRowCount(0);
    ui->experiment_2->setCurrentText(0);
    ui->classes_2->setText("2");
    ui->datapath_2->clear();
    ui->modelpath_2->clear();
}

void MainWindow::on_comboBox_activated(int index)
{
    ui->stackedWidgetRe->setCurrentIndex(index);
    if(index == 0){
        ui->loss_re->setVisible(true);
        ui->matrix->setVisible(false);
        QList<QString> names;
        QList<QPointF> data = getRecentData(names);
        drawLine(data, names, ui->advView);

        QVector<double> datalast = getLastData();
        drawBar(datalast, ui->barView);

    }else if(index == 1){
        ui->loss_re->setVisible(false);
        ui->matrix->setVisible(true);

    }

}


void MainWindow::on_matrix_clicked()
{
    QString filePath;
    filePath = QFileDialog::getOpenFileName(
           this,                               // 父窗口
           tr("选择文件"),                     // 对话框标题
           QDir::homePath(),                   // 默认打开路径（用户主目录）
           tr("csv文件 (*.csv)") // 文件过滤器
    );
    // 如果用户选择了文件
    if(filePath != ""){
        QVector<QPair<int, int>> data = readCSV(filePath);
        int numClasses = 47;
        if (filePath.contains("binary")) {
            numClasses = 2;  // 包含 "binary"
        } else if(filePath.contains("family")){
            numClasses = 696;  // 包含 "family"
        }

        QVector<QVector<int>> confusionMatrix = calculateConfusionMatrix(data, numClasses);
        // 绘制混淆矩阵
        createConfusionMatrixChart(confusionMatrix, numClasses);
    }

}

QVector<QPair<int, int>> MainWindow::readCSV(const QString &filePath) {
        QVector<QPair<int, int>> data;
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "Failed to open file:" << filePath;
            return data;
        }

        QTextStream in(&file);
        int lineCount = 0;
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (lineCount++ == 0) continue;  // 跳过标题行

            QStringList fields = line.split(",");
            if (fields.size() >= 3) {
                int actual = fields[1].toInt();
                int predicted = fields[2].toInt();
                data.append(qMakePair(actual, predicted));
            }
        }

        file.close();
        return data;
}

// 计算混淆矩阵
QVector<QVector<int>> MainWindow::calculateConfusionMatrix(const QVector<QPair<int, int>> &data, int numClasses) {
    QVector<QVector<int>> confusionMatrix(numClasses, QVector<int>(numClasses, 0));

    for (const auto &pair : data) {
        int actual = pair.first;
        int predicted = pair.second;
        if (actual >= 0 && actual < numClasses && predicted >= 0 && predicted < numClasses) {
            confusionMatrix[actual][predicted]++;
        }
    }

    return confusionMatrix;
}

// 创建混淆矩阵图表
void MainWindow::createConfusionMatrixChart(const QVector<QVector<int>> &confusionMatrix, int numClasses) {
    QTableWidget *tableWidget = ui->matrixtable;
    // 设置表格的行数和列数
    tableWidget->setRowCount(numClasses);
    tableWidget->setColumnCount(numClasses);
    // 设置表头标签
    QStringList labels;
    for (int i = 0; i < numClasses; ++i) {
        labels.append(QString::number(i));
    }
    tableWidget->setHorizontalHeaderLabels(labels);
    tableWidget->setVerticalHeaderLabels(labels);
    // 找到最大值用于归一化颜色
    int maxValue = 0;
    for (const auto &row : confusionMatrix) {
        for (int value : row) {
            if (value > maxValue) maxValue = value;
        }
    }

    // 定义颜色
    QColor white(255, 255, 255);      // #FFFFFF
    QColor lightBlue(122, 185, 254);  // #7AB9FE
    QColor darkBlue(52, 148, 254);    // #3494FE

    // 填充表格
    for (int i = 0; i < numClasses; ++i) {
        for (int j = 0; j < numClasses; ++j) {
            int value = confusionMatrix[i][j];
            QTableWidgetItem *item = new QTableWidgetItem(QString::number(value));
            item->setTextAlignment(Qt::AlignCenter);

            // 根据值设置背景颜色（白色、浅蓝色、深蓝色渐变）
            if (maxValue > 0) {
                double ratio = static_cast<double>(value) / maxValue;  // 归一化到[0, 1]

                QColor color;
                if (ratio < 0.5) {
                    // 白色到浅蓝色渐变
                    color.setRed(white.red() + static_cast<int>((lightBlue.red() - white.red()) * ratio * 2));
                    color.setGreen(white.green() + static_cast<int>((lightBlue.green() - white.green()) * ratio * 2));
                    color.setBlue(white.blue() + static_cast<int>((lightBlue.blue() - white.blue()) * ratio * 2));
                } else {
                    // 浅蓝色到深蓝色渐变
                    color.setRed(lightBlue.red() + static_cast<int>((darkBlue.red() - lightBlue.red()) * (ratio - 0.5) * 2));
                    color.setGreen(lightBlue.green() + static_cast<int>((darkBlue.green() - lightBlue.green()) * (ratio - 0.5) * 2));
                    color.setBlue(lightBlue.blue() + static_cast<int>((darkBlue.blue() - lightBlue.blue()) * (ratio - 0.5) * 2));
                }

                item->setBackground(color);
            }

            tableWidget->setItem(i, j, item);
        }
    }

    // 设置表格的拉伸模式
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);  // 水平拉伸
    tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);    // 垂直拉伸

    // 调整表格大小策略
    tableWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

}
// 生成类别标签
QStringList MainWindow::generateLabels(int numClasses) {
   QStringList labels;
   for (int i = 0; i < numClasses; ++i) {
       labels.append(QString::number(i));
   }
   return labels;
}


