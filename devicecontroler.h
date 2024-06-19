//
// Created by 神奇bug在哪里 on 24-6-19.
//

#ifndef CUIT_PRO_2_DEVICECONTROLER_H
#define CUIT_PRO_2_DEVICECONTROLER_H

#include <QWidget>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>


QT_BEGIN_NAMESPACE
namespace Ui { class deviceControler; }
QT_END_NAMESPACE

class deviceControler : public QWidget {
Q_OBJECT

public:
    explicit deviceControler(QWidget *parent = nullptr);

    ~deviceControler() override;

private:
    QSerialPort *m_serialPort;
private:
    Ui::deviceControler *ui;
public:
    void slots on_fanOnoff();

    void slots on_hexOnoff();

    void slots on_ledOnoff();

    void slots on_fanSpeed();

    void slots on_serialFresh();

    void slots on_serialReadable();

    signals:
    void deviceUpdate();

private:
    uint8_t deviceID = 0x05;
    bool fanOn = false;
    bool hexOn = false;
    bool lightOn = false;
    enum FanSpeed {
        fanSpeedLow = 0,
        fanSpeedMid = 1,
        fanSpeedHigh = 2
    };
    FanSpeed fanSpeed = fanSpeedLow;
private:
    bool fanToggle();

    bool hexToggle();

    bool lightToggle();

    bool setFanspeed(FanSpeed speed);

public:
    typedef struct DeviceStruct {
        uint8_t tag;
        uint8_t deviceID;
        uint8_t len;
        uint16_t tempVal;
        uint16_t humVal;
        uint8_t acc_x;
        uint8_t acc_y;
        uint8_t acc_z;
        uint32_t adc0Val;
        uint32_t adc1Val;
        uint32_t lightVal;
        struct state{
            uint8_t led;
            uint8_t fan;
            uint8_t buzz;
            uint8_t hex;
        }State;
    } DeviceStruct;
private:
    DeviceStruct deviceStatus;
public:
    DeviceStruct getDeviceStatus();
    QString getSerialPortName();
    uint8_t getDeviceId();
private: //helpers
    static uint16_t concat_bytes2(uint8_t low,uint8_t high);
    static uint32_t concat_bytes4(uint8_t n1,uint8_t n2,uint8_t n3,uint8_t n4);

};


#endif //CUIT_PRO_2_DEVICECONTROLER_H
