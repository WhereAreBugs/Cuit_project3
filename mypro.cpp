#include "mypro.h"
#include "./ui_mypro.h"
#include <QDebug>
MyPro::MyPro(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MyPro)
{
    ui->setupUi(this);
    // 将槽函数与信号关联
    connect(this, SIGNAL(send()), this, SLOT(receive()));
    connect(ui->pushButton, &QPushButton::clicked, this, &MyPro::send_submit);
}

MyPro::~MyPro()
{
    delete ui;
}
void MyPro::send_submit()
{
    qDebug() << "send_submit";
    emit send();
}
void MyPro::receive()
{
    qDebug() << "receive OK";
    ui->label->setText("receive OK");
}


