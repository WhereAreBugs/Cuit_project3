//
// Created by 神奇bug在哪里 on 2023/12/28.
//

// You may need to build the project (run Qt uic code generator) to get "ui_devTool.h" resolved

#include "devtool.h"
#include "ui_devTool.h"

devTool::devTool(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::devTool) {
    ui->setupUi(this);
    ui->showPaimeng->setPixmap(QPixmap(":/images/1.png"));
    ui->showPaimeng->setScaledContents(true);
    timer = new QTimer(this);
    timer->setInterval(100);

    connect(timer, &QTimer::timeout, this, [=]() {
        if (ui->buttonGroup->checkedId()==-6) {
            enableExtraSetting();
        } else {
            disableExtraSetting();
        }
        switch (ui->buttonGroup->checkedId()) {
            case  -1:case -2: //正常
            {
                this->temprature_min = 0;
                this->temprature_max = 40;
                this->humidity_min = 0;
                this->humidity_max = 100;
                break;
            }
            case -3: //低
            {
                this->temprature_min = 0;
                this->temprature_max = 20;
                this->humidity_min = 0;
                this->humidity_max = 30;
                break;
            }
            case -4: //正常范围
            {
                this->temprature_min = 20;
                this->temprature_max = 30;
                this->humidity_min = 30;
                this->humidity_max = 60;
                break;
            }
            case -5:
            {
                this->temprature_min = 30;
                this->temprature_max = 40;
                this->humidity_min = 60;
                this->humidity_max = 100;
                break;
            }
            case -6:
            {
                this->temprature_min = ui->tmpMinCus->text().toFloat();
                this->temprature_max = ui->tmpMaxCus->text().toFloat();
                this->humidity_min = ui->humMinCus->text().toFloat();
                this->humidity_max = ui->humMaxCus->text().toFloat();
                break;
            }
            default:
            {
                qDebug() << "[devtool] unknown button id";
            }
        }
        switch (ui->autoSaveSelec->checkedId()) {
            case  -1:case -2: //关闭自动保存数据
            {
                autoSaveMode = AutoSaveModeOff;
                break;
            }
            case -3: //低
            {
                autoSaveMode = AutoSaveModeOnLow;
                break;
            }
            case -4: //高
            {
                autoSaveMode = AutoSaveModeOnHIGH;
                break;
            }
            case -5: //全部非正常
            {
                autoSaveMode = AutoSaveModeOnNotOK;
                break;
            }
            case -6:
            {
                autoSaveMode = AutoSaveModeALL;
                break;
            }
            default:
            {
                qDebug() << "[devtool] unknown button id";
            }

        }
    });
    connect(ui->datasourceInput, &QLineEdit::editingFinished, this, [=]() {
        if (ui->datasourceInput->text().isEmpty()) {
            updateDataSrcUpdateTimeMs(500);
            return;
        }
        updateDataSrcUpdateTimeMs(ui->datasourceInput->text().toULongLong());
    });

    timer->start();
}

devTool::~devTool() {
    delete ui;
}

void devTool::updateDataSrcUpdateTimeMs(uint64_t ms) {
    this->dataSrcUpdateTimeMs = ms;
}

void devTool::disableExtraSetting() {
    ui->label_3->hide();
    ui->label_4->hide();
    ui->label_5->hide();
    ui->label_6->hide();
    ui->tmpMinCus->hide();
    ui->tmpMaxCus->hide();
    ui->humMinCus->hide();
    ui->humMaxCus->hide();
}

void devTool::enableExtraSetting() {
    ui->label_3->show();
    ui->label_4->show();
    ui->label_5->show();
    ui->label_6->show();
    ui->tmpMinCus->show();
    ui->tmpMaxCus->show();
    ui->humMinCus->show();
    ui->humMaxCus->show();
}
