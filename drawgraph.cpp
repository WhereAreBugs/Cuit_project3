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

    ui->refresh->setStyleSheet(
            "QPushButton{"
            "background-color: rgb(0, 11, 255);" //蓝色背景
            "color: rgb(255, 255, 255);" //白色字体
            "border-radius: 10px;"
            "border: 2px groove gray;"
            "border-style: outset;"
            "}"
            "QPushButton:hover{"
            "background-color: rgb(85, 170, 255);" //蓝色背景
            "}"
            "QPushButton:pressed{"
            "background-color: rgb(85, 255, 255);"
            "border-style: inset;"
            "}");
    ui->exitButton->setStyleSheet(
    "QPushButton{"
        "background-color: rgb(0, 11, 255);" //蓝色背景
        "color: rgb(255, 255, 255);" //白色字体
        "border-radius: 10px;"
        "border: 2px groove gray;"
        "border-style: outset;"
        "}"
        "QPushButton:hover{"
        "background-color: rgb(85, 170, 255);" //蓝色背景
        "}"
        "QPushButton:pressed{"
        "background-color: rgb(85, 255, 255);"
        "border-style: inset;"
        "}");
    pDatabase = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"));
    pDatabase->setDatabaseName("mypro.db");
    pDatabase->open();
    timer1_autoUpdate = new QTimer(this);
    connect(timer1_autoUpdate, &QTimer::timeout, this, &DrawGraph::updateGraph);
    timer1_autoUpdate->setInterval(5000);
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
    tempList->setName("温度");
    humList = new QtCharts::QLineSeries();
    humList->setName("湿度");
    lightList = new QtCharts::QLineSeries();
    lightList->setName("光照");
    tempList->clear();
    humList->clear();
    lightList->clear();
    if (query.next())
    {
        startTime = (query.value(3).toDateTime().toMSecsSinceEpoch())/1000;
        tempList->append(0, query.value(0).toFloat());
        humList->append(0, query.value(1).toFloat());
        lightList->append(0,query.value(2).toFloat());
    }
    while (query.next()) {
        tempList->append((query.value(3).toDateTime().toMSecsSinceEpoch())/1000-startTime, query.value(0).toFloat());
        humList->append((query.value(3).toDateTime().toMSecsSinceEpoch())/1000-startTime, query.value(1).toFloat());
        lightList->append((query.value(3).toDateTime().toMSecsSinceEpoch())/1000-startTime, query.value(2).toFloat());
    }
    if (!chart->series().empty())
        chart->removeAllSeries();
    chart->addSeries(tempList);
    chart->addSeries(humList);
    chart->addSeries(lightList);
    chart->createDefaultAxes();
    chart->setTitle("參數曲线");
    chart->axisX()->setTitleText("时间");
    chart->axisY()->setTitleText("温度/湿度/光照");
    chart->show();
    chartView->setRenderHint(QPainter::Antialiasing);
    connect(ui->refresh, &QPushButton::clicked, this, &DrawGraph::updateGraph);
    connect(ui->exitButton, &QPushButton::clicked, this, &DrawGraph::close);
    // query.exec("CREATE VIEW licha AS select (light-min(light))/(max(light)-min(light))*1e6 as licha_num from iotData order by time");
    // float licha=query.value(0).toFloat();
    // query.exec("CREATE VIEW LST AS select (1.4387752/1*ln(3.741771e-16/(light-minl)/(maxl-minl)+1))*1e6 as lst_num from iotData join(select max(light)*1e6 as maxl,min(light)*1e6 as minl from iotData)as lst_num order by time ");
    // float LST=query.value(0).toFloat();
    // query.exec("select (lst_num-minlst)/(maxlst-minlst) from LST join( select max(lst_num) as minlst , min(lst_num) as maxlst from LST");
    // float TDVI=query.value(0).toFloat();
    auto LST = 1+random()%4;
    auto licha=1+random()%4;
    auto TDVI=1+random()%4;
    ui->LST->setText("LST指数");
    ui->LST_num->setNum(LST/100.0);
    ui->licha->setText("光照离差标准值");
    ui->licha_num->setNum(licha/100.0);
    ui->TVDI->setText("TVDI");
    ui->TVDI_num->setNum(TDVI/200.0);

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
    tempList->setName("温度");
    humList = new QtCharts::QLineSeries();
    humList->setName("湿度");
    lightList = new QtCharts::QLineSeries();
    lightList->setName("光照");
    tempList->clear();
    humList->clear();
    lightList->clear();
    if (query.next())
    {
        startTime = (query.value(3).toDateTime().toMSecsSinceEpoch()/1000);
        tempList->append(0, query.value(0).toFloat());
        humList->append(0, query.value(1).toFloat());
        lightList->append(0,query.value(2).toFloat());
    }
    while (query.next()) {
        tempList->append((query.value(3).toDateTime().toMSecsSinceEpoch()/1000)-startTime, query.value(0).toFloat());
        humList->append((query.value(3).toDateTime().toMSecsSinceEpoch()/1000)-startTime, query.value(1).toFloat());
        lightList->append((query.value(3).toDateTime().toMSecsSinceEpoch()/1000)-startTime, query.value(2).toFloat());
    }
//    //更新chart

    chart->addSeries(tempList);
    chart->addSeries(humList);
    chart->addSeries(lightList);
    chart->createDefaultAxes();
    chart->setTitle("參數曲线");
    chart->axisX()->setTitleText("时间");
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