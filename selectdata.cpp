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
        baseQurey += " or temperature > 30";
    }
    if (ui->cB_LowTemp->isChecked()) {
        baseQurey += " or temperature < 20";
    }
    if (ui->cB_HighHum->isChecked()) {
        baseQurey += " or humidity > 60";
    }
    if (ui->cB_LowHum->isChecked()) {
        baseQurey += " or humidity < 30";
    }
    if (ui->cB_NormalHum->isChecked()) {
        baseQurey += " or humidity >= 30 and humidity <= 60";
    }
    if (ui->cB_NormalTemp->isChecked()) {
        baseQurey += " or temperature >= 20 and temperature <= 30";
    }
    QSqlQuery query(*pDatabase);
    query.exec(baseQurey);
    QSqlQueryModel *model = new QSqlQueryModel;
    model->setQuery(query);
    ui->tableView->setModel(model);
    ui->tableView->setColumnWidth(1, 40);
    ui->tableView->setColumnWidth(3, 40);
    ui->tableView->setColumnWidth(2, 260);
}
