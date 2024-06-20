//
// Created by 神奇bug在哪里 on 2023/12/29.
//

#ifndef CUIT_PRO_2_DRAWGRAPH_H
#define CUIT_PRO_2_DRAWGRAPH_H

#include <QMainWindow>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtCharts/QLineSeries>
#include <QChart>
#include <QChartView>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class DrawGraph; }
QT_END_NAMESPACE

class DrawGraph : public QMainWindow {
    Q_OBJECT

    public:
    explicit DrawGraph(QWidget *parent = nullptr);

    ~DrawGraph() override;

private:
    Ui::DrawGraph *ui;
    QtCharts::QLineSeries  *tempList;
    QtCharts::QLineSeries  *humList;
    QtCharts::QLineSeries  *lightList;
    QtCharts::QChart *chart = nullptr;
    QtCharts::QChartView *chartView;
    QSqlDatabase * pDatabase = nullptr;
    QTimer * timer1_autoUpdate = nullptr;
    QTimer * timer2_Light = nullptr;
    qint64 startTime;
    bool light = false;
    private Q_SLOTS:
        void updateGraph();
private:
    void ledUnset(QLabel *label, int size);
    void ledSet(QLabel* label, int color, int size);
};


#endif //CUIT_PRO_2_DRAWGRAPH_H
