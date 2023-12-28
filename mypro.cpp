#include "mypro.h"
#include "./ui_mypro.h"
#include <QDebug>
#include <QMovie>
#include <QStyleFactory>
#include <QGraphicsOpacityEffect>
#include "loginresult.h"
MyPro::MyPro(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MyPro)
{
    ui->setupUi(this);
    connect(ui->loginButton, &QPushButton::clicked, this, &MyPro::on_button1_clicked);
    QMovie *movie = new QMovie(":/images/3.gif");
    movie->start();
    ui->avator->setMovie(movie);
    ui->avator->setScaledContents(true);

    ui->passWord->setEchoMode(QLineEdit::Password);
    ui->passWord->setPlaceholderText(tr("请输入密码"));
    ui->passWord->setStyleSheet("QLineEdit{border: 1px solid rgb(200, 200, 200);"
                                "border-radius: 5px;"
                                "padding: 5px 10px;"
                                "font-size: 16px;"
                                "font-family: \"Microsoft YaHei\";}"
                                "QLineEdit:hover{border: 1px solid rgb(0, 170, 255);}"
                                "QLineEdit:focus{border: 1px solid rgb(0, 170, 255);}");


    ui->loginButton->setFlat(true);
    ui->loginButton->setText(tr("登录"));
    ui->loginButton->setStyleSheet("QPushButton{background-color: rgb(0, 85, 255);"
                                    "color: rgb(255, 255, 255);"
                                    "border-radius: 5px;"
                                    "border: 1px solid rgb(0, 85, 255);"
                                    "padding: 5px 10px;"
                                    "font-size: 16px;}"
                                    "QPushButton:hover{background-color: rgb(0, 170, 255);"
                                    "border: 1px solid rgb(0, 170, 255);}"
                                    "QPushButton:pressed{background-color: rgb(0, 170, 255);"
                                    "border: 1px solid rgb(0, 170, 255);}"
                                    "QPushButton:disabled{background-color: rgb(200, 200, 200);"
                                    "border: 1px solid rgb(200, 200, 200);}");

    ui->label_3->setText(tr("<a href=\"https://www.baidu.com\">扫码登录</a>"));
    ui->label_3->setStyleSheet("color: rgb(0, 85, 255);"
                                "font-size: 12px;"
                                "font-weight: bold;"
                                "font-family: \"Microsoft YaHei\";"
                                "text-decoration: none;");
    ui->label_4->setText(tr("<a href=\"https://www.baidu.com\">找回密码</a>"));
    ui->label_4->setStyleSheet("color: rgb(0, 85, 255);"
                                "font-size: 12px;"
                                "font-weight: bold;"
                                "font-family: \"Microsoft YaHei\";"
                                "text-decoration: none;");

    ui->label_3->setOpenExternalLinks(true);
    ui->label_4->setOpenExternalLinks(true);

}

MyPro::~MyPro()
{

    delete ui;
}

void MyPro::on_button1_clicked() {
    qDebug() << "button1 clicked";
    auto *loginResult = new LoginResult();
    loginResult->show();
    loginResult->setFocus();
    if (ui->userName->text().isEmpty() || ui->passWord->text().isEmpty()) {
        return;
    }
    emit loginResult->on_login();
}

