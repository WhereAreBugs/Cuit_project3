#ifndef MYPRO_H
#define MYPRO_H

#include <QMainWindow>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QTimer>
#include <QDateTime>
#include <QSqlQueryModel>
#include <QLabel>

#include "devtool.h"
#include "selectdata.h"
#include "drawgraph.h"
#include "devicecontroler.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MyPro;
}
QT_END_NAMESPACE

class MyPro : public QMainWindow
{
    Q_OBJECT

public:
    MyPro(QWidget *parent = nullptr);
    ~MyPro();

private://pointers
    Ui::MyPro *ui;
    QSqlDatabase *pDatabase = nullptr;
    QTimer *timer1_1sec = nullptr;
    QTimer * timer2_tempAndHumGen = nullptr;
    selectData *pSelectData = nullptr;
    QSqlQueryModel * model = nullptr;
    QString LCDStyleSheet = nullptr;
    devTool *pDevTool = nullptr;
    QTimer * timer3_updateTreeView = nullptr;
    QTimer * timer4_light = nullptr;
    QTimer * settingsCheckTimer = nullptr;
    deviceControler *pDeviceControler = nullptr;
    DrawGraph * pDrawGraphs = nullptr;
    QMovie * pMovie = nullptr;
private://helpers
    bool SQLUpdate = false;
    bool SrcUpdate = false;
    void lightTimerCallback();
private: //业务成员变量
    float temprature;
    float humidity;
    QDateTime eventTimeNow;
private://帮助函数
    void updateTreeView();
    static void ledSet(QLabel* label, int color, int size);
    static void ledUnset(QLabel* label, int size);
    friend class devTool;
private:
    void updateSrcUpdateTimeMs(uint64_t ms);

};
#endif // MYPRO_H
