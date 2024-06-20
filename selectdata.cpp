//
// Created by 神奇bug在哪里 on 2023/12/28.
//

// You may need to build the project (run Qt uic code generator) to get "ui_selectData.h" resolved

#include "selectdata.h"
#include "ui_selectData.h"
#include <QTimer>


selectData::selectData(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::selectData) {
    setWindowTitle("数据查询");
    ui->setupUi(this);
//
//    setStyleSheet("QWidget {"
//                  "border-image: url(:/images/6.jpg);"
//                  "}");
// 设置背景图片

    ui->bottom->setPixmap(QPixmap(":/images/6.png"));
    ui->bottom->setScaledContents(true);
    checkChecker = new QTimer(this);
    checkChecker->setInterval(50);
    connect(checkChecker, &QTimer::timeout, this, &selectData::checkFuncs);
    checkChecker->start();
    //初始化数据库
    pDatabase = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"));
    pDatabase->setDatabaseName("mypro.db");
    if (!pDatabase->open()) {
        qDebug() << "[selectData] database open failed";
    }

}

selectData::~selectData() {
    delete pDatabase;
    delete checkChecker;
    delete ui;
}

void selectData::checkFuncs() {
    QString baseQurey = "select temperature as 温度,humidity as 湿度,time as 时间 from iotData where false";
    if (ui->cB_HighTemp->isChecked()) {
        baseQurey += " or temperature > ";
        char bytes[5] = {0};
        sprintf(bytes, "%.1f", pDevTool->getTmpHighLimit());
        baseQurey += bytes;
    }
    if (ui->cB_LowTemp->isChecked()) {
        baseQurey += " or temperature < ";
        char bytes[5] = {0};
        sprintf(bytes, "%.1f", pDevTool->getTmpLowLimit());
        baseQurey += bytes;

    }
    if (ui->cB_HighHum->isChecked()) {
        baseQurey += " or humidity > ";
        char bytes[5] = {0};
        sprintf(bytes, "%.1f", pDevTool->getHumHighLimit());
        baseQurey += bytes;
    }
    if (ui->cB_LowHum->isChecked()) {
        baseQurey += " or humidity < ";
        char bytes[5] = {0};
        sprintf(bytes, "%.1f", pDevTool->getHumLowLimit());
        baseQurey += bytes;
    }
    if (ui->cB_NormalHum->isChecked()) {
        baseQurey += " or humidity >= ";
        char bytes[5] = {0};
        sprintf(bytes, "%.1f", pDevTool->getHumLowLimit());
        baseQurey += bytes;
        baseQurey += " and humidity <= ";
        sprintf(bytes, "%.1f", pDevTool->getHumHighLimit());
        baseQurey += bytes;
    }
    if (ui->cB_NormalTemp->isChecked()) {
        baseQurey += " or temperature >= ";
        char bytes[5] = {0};
        sprintf(bytes, "%.1f", pDevTool->getTmpLowLimit());
        baseQurey += bytes;
        baseQurey += " and temperature <= ";
        sprintf(bytes, "%.1f", pDevTool->getTmpHighLimit());
        baseQurey += bytes;
    }
    QSqlQuery query(*pDatabase);
    query.exec(baseQurey);
    auto *model = new QSqlQueryModel;
    model->setQuery(query);
    ui->tableView->setModel(model);
    ui->tableView->setColumnWidth(1, 40);
    ui->tableView->setColumnWidth(3, 40);
    ui->tableView->setColumnWidth(2, 260);
}

void selectData::setDevtool(devTool *pDevTool) {
    this->pDevTool = pDevTool;

}
