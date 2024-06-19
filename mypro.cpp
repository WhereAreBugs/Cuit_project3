#include "mypro.h"
#include "./ui_mypro.h"
#include "devicecontroler.h"
#include <QMovie>
#include <QGraphicsOpacityEffect>
#include <QDateTime>
#include <QTimer>
#include <QRandomGenerator>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-narrowing-conversions"

MyPro::MyPro(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MyPro) {
    setWindowTitle("物联网信息查询系统");
    ui->setupUi(this);
    //初始化背景
    setStyleSheet("QMainWindow {"
                  "border-image: url(:/images/7.png);"
                  "}");
    // 建立数据库链接
    pDatabase = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"));
    pDatabase->setDatabaseName("mypro.db");
    if (!pDatabase->open()) {
        qDebug() << "open database failed";
    }
    // 初始化数据表
    QSqlQuery query;
    //创建数据表:(主键，温度，湿度，时间)
    query.exec("create table if not exists iotData\n"
               "(\n"
               "    id          integer primary key autoincrement,\n"
               "    temperature float,\n"
               "    humidity    float,\n"
               "    light       integer,\n"
               "    adc0        integer,\n"
               "    adc1        integer,\n"
               "    time        datetime\n"
               ")");
    query.finish();
    // 数据表初始化完成
    // 初始化时钟
    ui->timeShow->setSegmentStyle(QLCDNumber::Flat);
    ui->timeShow->setStyleSheet("QLCDNumber {"
                                "border: 2px solid rgb(255, 170, 0);"
                                "border-radius: 10px;"
                                //                                "color: rgb(255, 170, 0);"
                                "}"); // 设置时钟的样式
    timer1_1sec = new QTimer(this);
    timer1_1sec->setInterval(std::chrono::seconds(1));
    connect(timer1_1sec, &QTimer::timeout, this, [=]() {
        QDateTime time = QDateTime::currentDateTime();
        QString str = time.toString("hh:mm:ss");
        ui->timeShow->display(str);
    });
    timer1_1sec->start();
    // 时钟初始化完成

    // 初始化LCD显示组件的样式
    LCDStyleSheet = "QLCDNumber {"
                    "border: 2px solid rgb(255, 170, 0);"
                    "border-radius: 10px;";

    // 初始化温度和湿度数据源
    timer2_tempAndHumGen = new QTimer(this);
    timer2_tempAndHumGen->setInterval(500);
    connect(timer2_tempAndHumGen, &QTimer::timeout, this, [=]() {
        if (!pDevTool->getDataSource()) {
            SrcUpdate = true;
            ledSet(ui->light_source, Qt::blue, 15);
            if (!timer4_light->isActive())
                timer4_light->start();

            temprature = QRandomGenerator::global()->
                    bounded(static_cast<int>(pDevTool->getTempratureMin()),
                            static_cast<int>(pDevTool->getTempratureMax()));
            humidity = QRandomGenerator::global()->
                    bounded(static_cast<int>(pDevTool->getHumidityMin()),
                            static_cast<int>(pDevTool->getHumidityMax()));
            eventTimeNow = QDateTime::currentDateTime();
            // 显示数据
            ui->tempratureDIsplay->display(temprature);
            ui->humityDisplay->display(humidity);
            // 显示数据完成
            checkDataRange();
        } else {
            eventTimeNow = QDateTime::currentDateTime();
        }
        ui->serialDisplay ->setText(pDeviceControler->getSerialPortName());
        ui->ID_Display->setText(QString('0'+pDeviceControler->getDeviceId()));
    });
    timer2_tempAndHumGen->start();
    pDeviceControler = new deviceControler();

    connect(pDeviceControler, &deviceControler::deviceUpdate, this, [=]() {
        if (!pDevTool->getDataSource())
            return;
        auto dev = pDeviceControler->getDeviceStatus();
        SrcUpdate = true;
        temprature = dev.tempVal;
        humidity = dev.humVal;
        light = dev.lightVal;
        adc0 = dev.adc0Val;
        adc1 = dev.adc1Val;
        ledSet(ui->light_source, Qt::blue, 15);
        if (!timer4_light->isActive())
            timer4_light->start();
        ui->tempratureDIsplay->display(dev.tempVal);
        ui->humityDisplay->display(dev.humVal);
        ui->light_display->display((int) dev.lightVal);
        ui->adc1_display->display((int) dev.adc0Val);
        ui->adc2_display->display((int) dev.adc1Val);
        checkDataRange();
    });
    // 初始化开发者工具界面
    pDevTool = new devTool();
    connect(ui->devloperTool, &QPushButton::clicked, this, [this]() {
        pDevTool->show();
        pDevTool->setFocus();
        settingsCheckTimer->start();
    });
    connect(pDevTool, &devTool::destroyed, this, [=]() {
        settingsCheckTimer->stop();
    });
    // 初始化开发者工具界面完成
    //初始化查询页面
    pSelectData = new selectData();

    // 设置保存按键
    connect(ui->recordButton, &QPushButton::clicked, this, [=]() {
        qDebug() << "save button clicked";
        QSqlQuery query;
        query.prepare("insert into iotData(temperature, humidity, light,adc0,adc1,time) values(?, ?, ?, ?, ?, ?)");
        query.addBindValue(temprature);
        query.addBindValue(humidity);
        query.addBindValue(light);
        query.addBindValue(adc0);
        query.addBindValue(adc1);
        query.addBindValue(eventTimeNow);
        query.exec();
        query.finish();
        // 更新treeview
        updateTreeView();
    });
    // 设置保存按键完成
    // 设置设备控制
    connect(ui->deviceControlButton, &QPushButton::clicked, this, [=]() {
        pDeviceControler->show();
        pDeviceControler->setFocus();
    });
    model = new QSqlQueryModel(ui->databaseShow);
    model->setQuery(
            "select id,temperature as 温度, humidity as 湿度, light as 光照, adc0, adc1, time as 时间 from iotData");
    ui->databaseShow->setModel(model);
    ui->databaseShow->setColumnHidden(0, true);
    ui->databaseShow->setColumnWidth(1, 40);
    ui->databaseShow->setColumnWidth(2, 40);
    ui->databaseShow->setColumnWidth(3, 40);
    ui->databaseShow->setColumnWidth(4, 40);
    ui->databaseShow->setColumnWidth(5, 40);
    ui->databaseShow->setColumnWidth(6, 260);
    // 设置treeview的样式
    ui->databaseShow->setStyleSheet("QTableView {"
                                    "border: 2px solid rgb(255, 170, 0);"
                                    "border-radius: 10px;"

                                    "}");
    ui->databaseShow->setAlternatingRowColors(true);
    ui->databaseShow->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->databaseShow->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->databaseShow->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // 在treeview中显示数据完成
    // 设置刷新按键
    connect(ui->refreshDatabse, &QPushButton::clicked, this, [=]() {
        updateTreeView();
    });
    connect(ui->deleteButton, &QPushButton::clicked, this, [=]() {
        auto selected = ui->databaseShow->selectionModel()->selectedRows();
        if (selected.empty()) {
            qDebug() << "no line selected";
            return;
        }
        QSqlQuery query;
        QString sql = "delete from iotData where false ";
        QList<int> idList;
        for (auto i: selected) {
            auto line = i.row();
            auto id = model->index(line, 0).data().toInt();
            qDebug() << "delete id:" << id;
            sql += " or id = ?";
            idList.append(id);
        }
        query.prepare(sql);
        for (auto &i: idList) {
            query.addBindValue(i);
        }
        query.exec();
        query.finish();
        updateTreeView();

    });
    // 设置刷新按键完成
    // 设置自动更新视图
    timer3_updateTreeView = new QTimer(this);
    timer3_updateTreeView->setInterval(std::chrono::seconds(10));
    connect(timer3_updateTreeView, &QTimer::timeout, this, [=]() {
        updateTreeView();
    });
    timer3_updateTreeView->start();
    // 设置自动更新视图完成
    // 设置指示灯
    ledSet(ui->light_SQL, Qt::green, 15);
    ledSet(ui->light_source, Qt::blue, 15);
    // 设置指示灯定时器
    timer4_light = new QTimer(this);
    timer4_light->setInterval(50);
    connect(timer4_light, &QTimer::timeout, this, [=]() {
        lightTimerCallback();
    });
    SQLUpdate = true;
    SrcUpdate = true;
    timer4_light->start();
    // 启动设置检查定时器
    settingsCheckTimer = new QTimer(this);
    settingsCheckTimer->setInterval(20);
    connect(settingsCheckTimer, &QTimer::timeout, this, [=]() {
        if (pDevTool->getDataSrcUpdateTimeMs() != timer2_tempAndHumGen->interval()) {
            updateSrcUpdateTimeMs(pDevTool->getDataSrcUpdateTimeMs());
        }
    });
    connect(ui->fliterDataButton, &QPushButton::clicked, this, [=]() {
        pSelectData->show();
        pSelectData->setFocus();
    });
    connect(ui->drawGraph, &QPushButton::clicked, this, [=]() {
        pDrawGraphs = new DrawGraph();
        pDrawGraphs->show();
        pDrawGraphs->setFocus();
    });
    connect(pDrawGraphs, &DrawGraph::destroyed, this, [=]() {
        delete pDrawGraphs;
    });
    //设置顶部按钮风格样式
    ui->fliterDataButton->setStyleSheet(
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
    ui->refreshDatabse->setStyleSheet(
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
    ui->recordButton->setStyleSheet(
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
    ui->deleteButton->setStyleSheet(
            "QPushButton{" //采用红色为主色调
            "background-color: rgb(255, 0, 0);" //红色背景
            "color: rgb(255, 255, 255);" //白色字体
            "border-radius: 10px;"
            "border: 2px groove gray;"
            "border-style: outset;"
            "}"
            "QPushButton:hover{"
            "background-color: rgb(255, 85, 127);" //深红色背景
            "}"
            "QPushButton:pressed{"
            "background-color: rgb(255, 170, 127);"
            "border-style: inset;"
            "}");
    ui->devloperTool->setStyleSheet(
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
    ui->deviceControlButton->setStyleSheet(
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
            "}"
    );
    ui->drawGraph->setStyleSheet(
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
    ui->WelcomeLable->setStyleSheet(
            "QLabel{"
            "color: rgb(0, 0, 0);" //黑色字体
            "font: 75 20pt \"微软雅黑\";"
            "border-radius: 10px;"
            "border: 2px groove gray;"
            "border-style: outset;"
            "border-color: rgb(255, 170, 0);"
            "}");
    pSelectData->setDevtool(pDevTool);
}

void MyPro::checkDataRange() {
    if (this->pDevTool->getAutoSaveMode() == devTool::AutoSaveModeOnNotOK) {
        if (this->humidity < this->pDevTool->getHumLowLimit() || this->humidity > this->pDevTool->getHumHighLimit() ||
            this->temprature < this->pDevTool->getTmpLowLimit() || this->temprature > this->pDevTool->getTmpHighLimit())
            this->ui->recordButton->click();
    } else if (this->pDevTool->getAutoSaveMode() == devTool::AutoSaveModeOnHIGH) {
        if (this->humidity > this->pDevTool->getHumHighLimit() || this->temprature > this->pDevTool->getTmpHighLimit())
            this->ui->recordButton->click();
    } else if (this->pDevTool->getAutoSaveMode() == devTool::AutoSaveModeOnLow) {
        if (this->humidity < this->pDevTool->getHumLowLimit() || this->temprature < this->pDevTool->getTmpHighLimit())
            this->ui->recordButton->click();
    } else if (this->pDevTool->getAutoSaveMode() == devTool::AutoSaveModeALL) {
        this->ui->recordButton->click();
    }

}

#pragma clang diagnostic pop

MyPro::~MyPro() {
    delete timer1_1sec;
    delete timer2_tempAndHumGen;
    delete timer3_updateTreeView;
    delete timer4_light;
    delete settingsCheckTimer;
    delete pDatabase;
    delete pDevTool;
    delete pDrawGraphs;
    delete pMovie;
    delete ui;
}

void MyPro::updateTreeView() {
    SQLUpdate = true;
    ledSet(ui->light_SQL, Qt::green, 15);
    if (!timer4_light->isActive())
        timer4_light->start();
    delete model;
    model = new QSqlQueryModel(ui->databaseShow);
    model->setQuery(
            "select id,temperature as 温度, humidity as 湿度, light as 光照, adc0, adc1 ,time as 时间 from iotData");
    ui->databaseShow->setModel(model);
    ui->databaseShow->setColumnHidden(0, true);
    ui->databaseShow->update();
}

void MyPro::lightTimerCallback() {
    if (SQLUpdate) {
        SQLUpdate = false;
        ledUnset(ui->light_SQL, 15);
    }
    if (SrcUpdate) {
        SrcUpdate = false;
        ledUnset(ui->light_source, 15);
    }
    timer4_light->stop();
}


void MyPro::ledSet(QLabel *label, int color, int size) {
    // 将label中的文字清空
    label->setText("");
    // 先设置矩形大小
    // 如果ui界面设置的label大小比最小宽度和高度小，矩形将被设置为最小宽度和最小高度；
    QString min_width = QString("min-width: %1px;").arg(size);              // 最小宽度：size
    QString min_height = QString("min-height: %1px;").arg(size);            // 最小高度：size
    QString max_width = QString("max-width: %1px;").arg(size);              // 最小宽度：size
    QString max_height = QString("max-height: %1px;").arg(size);            // 最小高度：size
    // 再设置边界形状及边框
    QString border_radius = QString("border-radius: %1px;").arg(size / 2);    // 边框是圆角，半径为size/2
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

void MyPro::ledUnset(QLabel *label, int size) {
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
    QString border_radius = QString("border-radius: %1px;").arg(size / 2);    // 边框是圆角，半径为size/2
    QString border = QString("border:1px solid black;");                    // 边框为1px黑色

    const QString SheetStyle = min_width + min_height + max_width + max_height + border_radius + border;
    label->setStyleSheet(SheetStyle);

}

void MyPro::updateSrcUpdateTimeMs(uint64_t ms) {
    timer2_tempAndHumGen->stop();
    timer2_tempAndHumGen->setInterval(static_cast<int >(ms));
    timer2_tempAndHumGen->start();
}


