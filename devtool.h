//
// Created by 神奇bug在哪里 on 2023/12/28.
//

#ifndef CUIT_PRO_2_DEVTOOL_H
#define CUIT_PRO_2_DEVTOOL_H

#include <QMainWindow>
#include <QTimer>
#include <QDebug>


QT_BEGIN_NAMESPACE
namespace Ui { class devTool; }
QT_END_NAMESPACE

class devTool : public QMainWindow {
Q_OBJECT

public:
    explicit devTool(QWidget *parent = nullptr);

    ~devTool() override;

    float getTempratureMin() const{return temprature_min;}
    float getTempratureMax() const{return temprature_max;}
    float getHumidityMin() const{return humidity_min;}
    float getHumidityMax() const{return humidity_max;}
    uint64_t getDataSrcUpdateTimeMs() const{return dataSrcUpdateTimeMs;}
public:
    enum AutoSaveMode{
        AutoSaveModeOff = -1,
        AutoSaveModeOnLow = -2,
        AutoSaveModeOnHIGH = -3,
        AutoSaveModeOnNotOK = -4,
        AutoSaveModeALL = -5
    };
    AutoSaveMode getAutoSaveMode() const{return autoSaveMode;}
    bool getDataSource() const;
private:

    Ui::devTool *ui;
    QTimer *timer;
    float temprature_min=0;
    float temprature_max=40;
    float humidity_min=0;
    float humidity_max=100;
    uint64_t  dataSrcUpdateTimeMs= 500;
    void updateDataSrcUpdateTimeMs(uint64_t ms);
    AutoSaveMode autoSaveMode = AutoSaveModeOff;
    void disableExtraSetting();
    void enableExtraSetting();
};


#endif //CUIT_PRO_2_DEVTOOL_H
