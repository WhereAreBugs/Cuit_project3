#ifndef MYPRO_H
#define MYPRO_H

#include <QMainWindow>

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

signals:
    void send();

public slots:
    void send_submit();
    void receive();

private:
    Ui::MyPro *ui;
};
#endif // MYPRO_H
