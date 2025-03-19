#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QtCharts>
using namespace QtCharts;

#include <QMainWindow>
#include <QFileDialog>
#include <QProcess>
#include <QDebug>
#include <QVector>
#include <QMessageBox>
#include <QPixmap>
//#include "./cnpy-master/cnpy.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void tabswitch(int num);

    void dataswitch(int num);

    void on_malwarepb_clicked();

    void on_benignpb_clicked();

    void on_pthpb_clicked();

    void on_flashpb_clicked();

    void drawLine(QList<QPointF> &p, QList<QString> names, QChartView *view);
    void drawSingleLine(QString csvpath, QChartView *view);
    void drawBar(QVector<double> &p, QChartView *view);

    QVector<QPair<int, int>> readCSV(const QString &filePath);
    QVector<QVector<int>> calculateConfusionMatrix(const QVector<QPair<int, int>> &data, int numClasses);
    void createConfusionMatrixChart(const QVector<QVector<int>> &confusionMatrix, int numClasses);
    QStringList generateLabels(int numClasses);

    QList<QPointF> getRecentData(QList<QString> &names);
    QVector<double> getLastData();

    void initnpyTable(QTableWidget *table);
    void loadCsvFile(QString csvPath, QTableWidget *table);
    void npyToCsv(QString path);
    void showDetectResult(QString path);
    void initDetectTable();

    //进程
    void startProcess(QString gen="test");  // 启动进程
    void generateProcess(QString gen="generate");  // 启动进程
    void readOutput(QTextEdit *text);    // 读取标准输出
    void readError(QTextEdit *text);     // 读取错误输出
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus, QTextEdit *text);  // 进程结束

    void on_npyrun_clicked();

    void on_apkrun_clicked();

    void on_datacombox_activated(int index);

    void on_apkchs_clicked();

    void on_apktrans_clicked();

    void on_loss_re_clicked();

    void on_advpb1_clicked();

    void on_advpb2_clicked();

    void on_advpb3_clicked();

    void on_clearpb_clicked();

    void on_modelpb_clicked();

    void on_datapb_clicked();

    void on_experiment_activated(int index);

    void on_detectpb_clicked();


    void on_stoppb_clicked();

    void on_experiment_2_activated(int index);

    void on_detectpb_2_clicked();

    void on_stoppb_2_clicked();

    void on_clearpb_2_clicked();

    void on_comboBox_activated(int index);


    void on_matrix_clicked();

private:
    Ui::MainWindow *ui;
    QProcess *process, *process2;
};
#endif // MAINWINDOW_H
