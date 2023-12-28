#include "mypro.h"
#include "./ui_mypro.h"
#include <QDebug>
#include <QMovie>
#include <QGraphicsOpacityEffect>
#include <QDateTime>
#include <QTimer>
#include <QRandomGenerator>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

MyPro::MyPro(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MyPro)
{
    ui->setupUi(this);
    ui->calendarWidget->setGridVisible(true);
    connect(ui->calendarWidget, SIGNAL(clicked(const QDate &)), this, SLOT(on_cal_clicked(const QDate &)));
    ui->calendarWidget->setMinimumDate(QDate(2021, 1, 1));
    ui->calendarWidget->setMaximumDate(QDate(2025, 12, 31));
    ui->calendarWidget->setFirstDayOfWeek(Qt::Monday);
    connect(ui->setTimeDateButton, SIGNAL(clicked()), this, SLOT(on_timeDate_set_clicked()));
    connect(ui->setDateButton, SIGNAL(clicked()), this, SLOT(on_date_set_clicked()));
    connect(ui->setTimeButton, SIGNAL(clicked()), this, SLOT(on_time_set_clicked()));
    auto *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(on_timer_tiggered()));
    timer->start(1000);
    connect(ui->getDateButton, SIGNAL(clicked()), this, SLOT(on_getDateButton_clicked()));
    connect(ui->getTimeButton, SIGNAL(clicked()), this, SLOT(on_getTimeButton_clicked()));
    connect(ui->getDateTImeButton, SIGNAL(clicked()), this, SLOT(on_getDateTimeButton_clicked()));
    ui->dateEdit->setDate(QDate::currentDate());
    ui->timeEdit->setTime(QTime::currentTime());
    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
    ui->dateShow_2->setReadOnly(true);
    ui->timeShow_2->setReadOnly(true);
    ui->timeDateShow_2->setReadOnly(true);
//    ui->dateTimeEdit->setReadOnly(true);
//    ui->dateEdit->setReadOnly(true);
//    ui->timeEdit->setReadOnly(true);

}

MyPro::~MyPro()
{

    delete ui;
}

void MyPro::on_cal_clicked(const QDate &date) {
    qDebug() << "cal clicked";
    ui->calendarShow->setText(date.toString("yyyy-MM-dd"));
}

void MyPro::on_timeDate_set_clicked() {
    qDebug() << "timeDate_set clicked";
    QDateTime dateTime = QDateTime::fromString(ui->timeDateShow->text(),"yyyy-MM-dd hh:mm:ss");
    ui->dateTimeEdit->setDateTime(dateTime);
}

void MyPro::on_date_set_clicked() {
    qDebug() << "date_set clicked";
    QDate date = QDate::fromString(ui->dateShow->text(),"yyyy-MM-dd");
    ui->dateEdit->setDate(date);


}

void MyPro::on_time_set_clicked() {
    qDebug() << "time_set clicked";
    QTime time = QTime::fromString(ui->timeShow->text(),"hh:mm:ss");
    ui->timeEdit->setTime(time);

}

void MyPro::on_timer_tiggered() {
    QDateTime now = QDateTime::currentDateTime();
    ui->realTimeShow->setText(now.toString("yyyy-MM-dd hh:mm:ss"));
    //在lcdNumber上显示时间
    ui->lcdNumber->display(now.toString("hh:mm:ss"));

}

void MyPro::on_getDateButton_clicked() {
    qDebug() << "getDateButton clicked";
    QDate date = QDate::currentDate();
    ui->dateShow_2->setText(date.toString("yyyy-MM-dd"));
}

void MyPro::on_getTimeButton_clicked() {
    qDebug() << "getTimeButton clicked";
    QTime time = QTime::currentTime();
    ui->timeShow_2->setText(time.toString("hh:mm:ss"));

}

void MyPro::on_getDateTimeButton_clicked() {
    qDebug() << "getDateTimeButton clicked";
    QDateTime dateTime = QDateTime::currentDateTime();
    ui->timeDateShow_2->setText(dateTime.toString("yyyy-MM-dd hh:mm:ss"));

}



