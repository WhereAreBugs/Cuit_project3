//
// Created by 神奇bug在哪里 on 2023/12/29.
//

// You may need to build the project (run Qt uic code generator) to get "ui_DrawGraph.h" resolved

#include "drawgraph.h"
#include "ui_DrawGraph.h"
#include <QDateTime>
#include <QChartView>
#include <QTimer>
#include <QDebug>
DrawGraph::DrawGraph(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::DrawGraph) {
    ui->setupUi(this);
    pDatabase = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"));
    pDatabase->setDatabaseName("mypro.db");
    pDatabase->open();
    timer1_autoUpdate = new QTimer(this);
    connect(timer1_autoUpdate, &QTimer::timeout, this, &DrawGraph::updateGraph);
    timer1_autoUpdate->setInterval(500);
    timer1_autoUpdate->start();
    timer2_Light = new QTimer(this);
    connect(timer2_Light, &QTimer::timeout, this, [=](){
        if (light)
        {
            ledUnset(ui->lightInd, 15);
            light = false;
        }
        timer2_Light->stop();
    });
    timer2_Light->setInterval(50);
    chart = new QtCharts::QChart();
    chartView = new QtCharts::QChartView(chart);
    ui->verticalLayout->addWidget(chartView);
    light = true;
    ledSet(ui->lightInd, Qt::blue, 15);
    timer2_Light->start();
    QSqlQuery query(*pDatabase);
    query.exec("select temperature,humidity,light,time from iotData");
    tempList = new QtCharts::QLineSeries();
    humList = new QtCharts::QLineSeries();
    lightList = new QtCharts::QLineSeries();
    tempList->clear();
    humList->clear();
    lightList->clear();
    if (query.next())
    {
        startTime = query.value(3).toDateTime().toSecsSinceEpoch();
        tempList->append(0, query.value(0).toFloat());
        humList->append(0, query.value(1).toFloat());
        lightList->append(0, query.value(2).toFloat());
    }
    while (query.next()) {
        tempList->append(query.value(3).toDateTime().toSecsSinceEpoch()-startTime, query.value(0).toFloat());
        humList->append(query.value(3).toDateTime().toSecsSinceEpoch()-startTime, query.value(1).toFloat());
        lightList->append(query.value(3).toDateTime().toSecsSinceEpoch()-startTime, query.value(2).toFloat());
           }
    chart->legend()->hide();
    if (!chart->series().empty())
        chart->removeAllSeries();
    chart->addSeries(tempList);
    chart->addSeries(humList);
    chart->addSeries(lightList);
    chart->createDefaultAxes();
    chart->setTitle("参数曲线");
//    chart->axisX()->setTitleText("时间");
//    chart->axisY()->setTitleText("温度/湿度");
    chart->axisY()->setTitleText("温度/湿度/光照");
    chart->show();
    chartView->setRenderHint(QPainter::Antialiasing);
    connect(ui->refresh, &QPushButton::clicked, this, &DrawGraph::updateGraph);
}

DrawGraph::~DrawGraph() {
    delete pDatabase;
    delete ui;
    delete chart;
    delete chartView;
    delete timer1_autoUpdate;
    delete timer2_Light;

}

void DrawGraph::updateGraph() {
    light = true;
    ledSet(ui->lightInd, Qt::blue, 15);
    timer2_Light->start();
    QSqlQuery query(*pDatabase);
    chart->removeAllSeries();
    query.exec("select temperature,humidity,light,time from iotData");
    tempList = new QtCharts::QLineSeries();
    humList = new QtCharts::QLineSeries();
    lightList = new QtCharts::QLineSeries();
    tempList->clear();
    humList->clear();
    lightList->clear();
    if (query.next())
    {
        startTime = query.value(3).toDateTime().toSecsSinceEpoch();
        tempList->append(0, query.value(0).toFloat());
        humList->append(0, query.value(1).toFloat());
        lightList->append(0, query.value(2).toFloat());
    }
    while (query.next()) {
        tempList->append(query.value(3).toDateTime().toSecsSinceEpoch()-startTime, query.value(0).toFloat());
        humList->append(query.value(3).toDateTime().toSecsSinceEpoch()-startTime, query.value(1).toFloat());
        lightList->append(query.value(3).toDateTime().toSecsSinceEpoch()-startTime, query.value(2).toFloat());
          }
//    //更新chart
    chart->addSeries(tempList);
    chart->addSeries(humList);
    chart->addSeries(lightList);
    chart->createDefaultAxes();
    chart->setTitle("参数曲线");
//    chart->axisX()->setTitleText("时间");
    chart->axisY()->setTitleText("温度/湿度/光照");
    chart->show();
    chartView->setChart(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->repaint();
    chartView->update();
    chartView->show();
}

void DrawGraph::ledSet(QLabel* label, int color, int size)
{
    // 将label中的文字清空
    label->setText("");
    // 先设置矩形大小
    // 如果ui界面设置的label大小比最小宽度和高度小，矩形将被设置为最小宽度和最小高度；
    // 如果ui界面设置的label大小比最小宽度和高度大，矩形将被设置为最大宽度和最大高度；
    QString min_width = QString("min-width: %1px;").arg(size);              // 最小宽度：size
    QString min_height = QString("min-height: %1px;").arg(size);            // 最小高度：size
    QString max_width = QString("max-width: %1px;").arg(size);              // 最小宽度：size
    QString max_height = QString("max-height: %1px;").arg(size);            // 最小高度：size
    // 再设置边界形状及边框
    QString border_radius = QString("border-radius: %1px;").arg(size/2);    // 边框是圆角，半径为size/2
    QString border = QString("border:1px solid black;");                    // 边框为1px黑色
    // 最后设置背景颜色
    QString background = "background-color:";
    switch (color) {
        case Qt::red:
            background += "red;";
            break;
        case Qt::green:
            background += "green;";
            break;
        case Qt::blue:
            background += "blue;";
            break;
        case Qt::yellow:
            background += "yellow;";
            break;
        case Qt::gray:
            background += "gray;";
            break;
        default:
            background += "black;";
            break;
    }

    const QString SheetStyle = min_width + min_height + max_width + max_height + border_radius + border + background;
    label->setStyleSheet(SheetStyle);

}

void DrawGraph::ledUnset(QLabel *label, int size) {
    // 将label中的文字清空
    label->setText("");
    // 先设置矩形大小
    // 如果ui界面设置的label大小比最小宽度和高度小，矩形将被设置为最小宽度和最小高度；
    // 如果ui界面设置的label大小比最小宽度和高度大，矩形将被设置为最大宽度和最大高度；
    QString min_width = QString("min-width: %1px;").arg(size);              // 最小宽度：size
    QString min_height = QString("min-height: %1px;").arg(size);            // 最小高度：size
    QString max_width = QString("max-width: %1px;").arg(size);              // 最小宽度：size
    QString max_height = QString("max-height: %1px;").arg(size);            // 最小高度：size
    // 再设置边界形状及边框
    QString border_radius = QString("border-radius: %1px;").arg(size/2);    // 边框是圆角，半径为size/2
    QString border = QString("border:1px solid black;");                    // 边框为1px黑色

    const QString SheetStyle = min_width + min_height + max_width + max_height + border_radius + border;
    label->setStyleSheet(SheetStyle);

}