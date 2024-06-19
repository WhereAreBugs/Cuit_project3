//
// Created by 神奇bug在哪里 on 24-6-19.
//

// You may need to build the project (run Qt uic code generator) to get "ui_deviceControler.h" resolved

#include "devicecontroler.h"
#include "ui_deviceControler.h"
#include <qtimer.h>


deviceControler::deviceControler(QWidget *parent) :
        QWidget(parent), ui(new Ui::deviceControler) {
    ui->setupUi(this);
    m_serialPort = new QSerialPort(this);
    m_serialPort->setBaudRate(115200, QSerialPort::AllDirections);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);
    m_serialPort->setDataBits(QSerialPort::Data8);
    QSerialPortInfo ports;
    for (const QSerialPortInfo &info: QSerialPortInfo::availablePorts()) {
        ui->serialList->addItem(info.portName());
        if (info.portName().contains("tty") && (info.portName().contains("USB") || info.portName().contains("usb"))){
            m_serialPort->setPortName(info.portName());
            m_serialPort->open(QSerialPort::ReadWrite);
        }
    }
    ui->IDset->setText(QString(deviceID));
    auto *serialRefreshTimer = new QTimer(this);
    serialRefreshTimer->start(500);
    connect(serialRefreshTimer, &QTimer::timeout, this, &deviceControler::on_serialFresh);
    connect(ui->fanOnoff, &QPushButton::clicked, this, &deviceControler::on_fanOnoff);
    connect(ui->FanSpeed, &QDial::sliderMoved, this, &deviceControler::on_fanSpeed);
    connect(ui->serialList, &QListWidget::itemClicked, this, [=]() {
        m_serialPort->setPortName(ui->serialList->currentItem()->text());
        m_serialPort->open(QSerialPort::ReadWrite);
    });
    connect(ui->hexOnoff, &QPushButton::clicked, this, &deviceControler::on_hexOnoff);
    connect(ui->ledOnoff, &QPushButton::clicked, this, &deviceControler::on_ledOnoff);
    connect(m_serialPort, &QSerialPort::readyRead, this, &deviceControler::on_serialReadable);
    connect(ui->IDset, &QLineEdit::editingFinished, this, [=] {
        deviceID = static_cast<uint8_t>(ui->IDset->text().toInt());
    });
    hexOn = true;
    hexToggle();
}

deviceControler::~deviceControler() {
    delete m_serialPort;
    delete ui;
}

void deviceControler::on_fanOnoff() {
    fanToggle();
}

void deviceControler::on_serialFresh() {
    QSerialPortInfo ports;
    ui->serialList->clear();
    for (const QSerialPortInfo &info: QSerialPortInfo::availablePorts()) {
        ui->serialList->addItem(info.portName());
    }
}

bool deviceControler::fanToggle() {
    uint8_t rawData[5] = {0xDD, 0, 0x24, 0x00, 0};
    rawData[1] = deviceID;
    rawData[4] = 0x04 + 0x04 * (!fanOn);
    m_serialPort->write(reinterpret_cast<const char *>(rawData), 5);
    return true;
}

bool deviceControler::hexToggle() {
    uint8_t rawData[5] = {0xDD, 0, 0x24, 0x00, 0};
    rawData[1] = deviceID;
    rawData[4] = 0x09 + !hexOn;
    m_serialPort->write(reinterpret_cast<const char *>(rawData), 5);
    return true;
}

bool deviceControler::lightToggle() {
    uint8_t rawData[5] = {0xDD, 0, 0x24, 0x00, 0};
    rawData[1] = deviceID;
    rawData[4] = 0x00 + !lightOn;
    m_serialPort->write(reinterpret_cast<const char *>(rawData), 5);
    return true;
}

bool deviceControler::setFanspeed(deviceControler::FanSpeed speed) {
    uint8_t rawData[5] = {0xDD, 0, 0x24, 0x00, 0};
    rawData[1] = deviceID;
    rawData[4] = 0x05 + speed;
    m_serialPort->write(reinterpret_cast<const char *>(rawData), 5);
    return true;
}

void deviceControler::on_hexOnoff() {
    hexToggle();
}

void deviceControler::on_ledOnoff() {
    lightToggle();
}

void deviceControler::on_fanSpeed() {
    FanSpeed newSpeed = static_cast<FanSpeed>(ui->FanSpeed->value());
    fanSpeed = newSpeed;
    setFanspeed(newSpeed);
}

void deviceControler::on_serialReadable() {
    unsigned char ref = static_cast<uint8_t>(m_serialPort->read(1)[0]);
    if (ref == 0xBB) {
        QByteArray raw = m_serialPort->read(35);
        if (raw[0] == static_cast<char>(deviceID)) {
            deviceStatus.deviceID = raw[0];
            deviceStatus.len = raw[1];
            deviceStatus.tempVal = concat_bytes2(raw[3], raw[4]);
            deviceStatus.humVal = concat_bytes2(raw[5], raw[6]);
            deviceStatus.acc_x = raw[7];
            deviceStatus.acc_y = raw[8];
            deviceStatus.acc_z = raw[9];
            deviceStatus.adc0Val = concat_bytes4(raw[11], raw[12], raw[13], raw[14]);
            deviceStatus.adc1Val = concat_bytes4(raw[15], raw[16], raw[17], raw[18]);
            deviceStatus.lightVal = concat_bytes4(raw[19], raw[20], raw[21], raw[22]);
            deviceStatus.State.led = raw[23];
            deviceStatus.State.fan = raw[24];
            deviceStatus.State.buzz = raw[25];
            deviceStatus.State.hex = raw[26];
            emit deviceUpdate();
        }
    }
    m_serialPort->readAll();
}

uint16_t deviceControler::concat_bytes2(uint8_t high, uint8_t low) {
    return (high << 8) | low;
}

uint32_t deviceControler::concat_bytes4(uint8_t n1, uint8_t n2, uint8_t n3, uint8_t n4) {
    return (n4 << 24) | (n3 << 16) | (n2 << 8) | n1;
}

deviceControler::DeviceStruct deviceControler::getDeviceStatus() {
    return deviceStatus;
}

QString deviceControler::getSerialPortName() {
    return m_serialPort->portName();
}

uint8_t deviceControler::getDeviceId() {
    return deviceID;
}







