//
// Created by 神奇bug在哪里 on 2023/12/27.
//

// You may need to build the project (run Qt uic code generator) to get "ui_LoginResult.h" resolved

#include "loginresult.h"
#include "ui_LoginResult.h"


LoginResult::LoginResult(QWidget *parent) :
        QWidget(parent), ui(new Ui::LoginResult) {
    ui->setupUi(this);
    connect(this, &LoginResult::on_login, this, &LoginResult::on_login_success);
    ui->label->setText("登录失败");
    ui->avator->setText("你有猫饼");
    ui->avator->setStyleSheet("QLabel{background-color: rgb(0, 85, 255);"
                              "color: rgb(255, 255, 255);"
                              "border-radius: 5px;"
                              "border: 1px solid rgb(0, 85, 255);"
                              "padding: 5px 10px;"
                              "font-size: 20px;}");
}

LoginResult::~LoginResult() {
    delete ui;
}

void LoginResult::on_login_success() {
    ui->label->setText("登录成功");
    ui->avator->setPixmap(QPixmap(":/images/1.jpg"));
    ui->avator->setScaledContents(true);

    QPixmap pixmap(":/images/1.png");
    pixmap.scaled(ui->avator->size(), Qt::KeepAspectRatio);
    ui->avator->setPixmap(pixmap);
    ui->avator->setScaledContents(true);

}
