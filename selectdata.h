//
// Created by 神奇bug在哪里 on 2023/12/28.
//

#ifndef CUIT_PRO_2_SELECTDATA_H
#define CUIT_PRO_2_SELECTDATA_H

#include <QMainWindow>
#include "QtSql"

QT_BEGIN_NAMESPACE
namespace Ui { class selectData; }
QT_END_NAMESPACE

class selectData : public QMainWindow {
Q_OBJECT

public:
    explicit selectData(QWidget *parent = nullptr);

    ~selectData() override;
private Q_SLOTS:
    void checkFuncs();
private:
    Ui::selectData *ui;
    QTimer * checkChecker = nullptr;
    QSqlDatabase * pDatabase = nullptr;
};


#endif //CUIT_PRO_2_SELECTDATA_H
