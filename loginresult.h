//
// Created by 神奇bug在哪里 on 2023/12/27.
//

#ifndef CUIT_PRO_2_LOGINRESULT_H
#define CUIT_PRO_2_LOGINRESULT_H

#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui { class LoginResult; }
QT_END_NAMESPACE

class LoginResult : public QWidget {
Q_OBJECT

public:
    explicit LoginResult(QWidget *parent = nullptr);

    ~LoginResult() override;
signals:
    void on_login();
public slots:
 void on_login_success();
private:
    Ui::LoginResult *ui;
};


#endif //CUIT_PRO_2_LOGINRESULT_H
