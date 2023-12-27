#include "mypro.h"
#include "./ui_mypro.h"
#include <QDebug>
MyPro::MyPro(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MyPro)
{
    ui->setupUi(this);
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(on_button1_clicked()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(on_button2_clicked()));
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(on_button3_clicked()));
}

MyPro::~MyPro()
{
    delete ui;
}

void MyPro::on_button1_clicked() {
    qDebug() << "button1 clicked";
    ui->label_CUIT->setText("我爱成信工");
}

void MyPro::on_button2_clicked() {
    qDebug() << "button2 clicked";
    ui->lineEdit->setEchoMode(QLineEdit::Password);
    ui->lineEdit->setText("123456");
}

void MyPro::on_button3_clicked() {
    qDebug() << "button3 clicked";
    ui->progressBar->setRange(0,100);
    ui->progressBar->setValue(60);
}

