#include "mypro.h"
#include "./ui_mypro.h"
#include <QDebug>
#include <QMovie>

MyPro::MyPro(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MyPro)
{
    ui->setupUi(this);
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(on_button1_clicked()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(on_button2_clicked()));
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(on_button3_clicked()));
    QPixmap pixmap(":/images/1.png");
    pixmap.scaled(ui->label_clor->size(), Qt::AspectRatioMode::KeepAspectRatio);
    ui->label_clor->setPixmap(pixmap);
    ui->label_clor->setScaledContents(true);
    QMovie movie(":/images/3.gif");
    movie.start();
    movie.setScaledSize(ui->label_movie->size());
    ui->label_movie->setMovie(&movie);
    ui->label_movie->setScaledContents(true);
    ui->label_movie->setAlignment(Qt::AlignCenter);
    ui->extern_2->setText(tr("<a href=\"https://www.baidu.com\">百度一下</a>"));
    ui->extern_2->setOpenExternalLinks(true);
    ui->extern_2->setAlignment(Qt::AlignCenter);
    ui->extern_2->setToolTip(tr("百度一下"));

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

