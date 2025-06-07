//
// Created by 神奇bug在哪里 on 24-6-19.
//

// You may need to build the project (run Qt uic code generator) to get "ui_deviceControler.h" resolved

#include "devicecontroler.h"
#include "ui_deviceControler.h"
#include <qtimer.h>
#include <QDebug>


deviceControler::deviceControler(QWidget *parent) : QWidget(parent), ui(new Ui::deviceControler) {
    ui->setupUi(this);


    ui->fanOnoff->setStyleSheet(
        "QPushButton{"
        "background-color: rgb(0, 11, 255);" //蓝色背景
        "color: rgb(255, 255, 255);" //白色字体
        "border-radius: 10px;"
        "border: 2px groove gray;"
        "border-style: outset;"
        "}"
        "QPushButton:hover{"
        "background-color: rgb(85, 170, 255);" //蓝色背景
        "}"
        "QPushButton:pressed{"
        "background-color: rgb(85, 255, 255);"
        "border-style: inset;"
        "}");
    ui->hexOnoff->setStyleSheet(
        "QPushButton{"
        "background-color: rgb(0, 11, 255);" //蓝色背景
        "color: rgb(255, 255, 255);" //白色字体
        "border-radius: 10px;"
        "border: 2px groove gray;"
        "border-style: outset;"
        "}"
        "QPushButton:hover{"
        "background-color: rgb(85, 170, 255);" //蓝色背景
        "}"
        "QPushButton:pressed{"
        "background-color: rgb(85, 255, 255);"
        "border-style: inset;"
        "}");
    ui->ledOnoff->setStyleSheet(
        "QPushButton{"
        "background-color: rgb(0, 11, 255);" //蓝色背景
        "color: rgb(255, 255, 255);" //白色字体
        "border-radius: 10px;"
        "border: 2px groove gray;"
        "border-style: outset;"
        "}"
        "QPushButton:hover{"
        "background-color: rgb(85, 170, 255);" //蓝色背景
        "}"
        "QPushButton:pressed{"
        "background-color: rgb(85, 255, 255);"
        "border-style: inset;"
        "}");
    ui->buzzContorl->setStyleSheet("QPushButton{"
        "background-color: rgb(0, 11, 255);" //蓝色背景
        "color: rgb(255, 255, 255);" //白色字体
        "border-radius: 10px;"
        "border: 2px groove gray;"
        "border-style: outset;"
        "}"
        "QPushButton:hover{"
        "background-color: rgb(85, 170, 255);" //蓝色背景
        "}"
        "QPushButton:pressed{"
        "background-color: rgb(85, 255, 255);"
        "border-style: inset;"
        "}");
    ui->exitButton->setStyleSheet("QPushButton{"
        "background-color: rgb(0, 11, 255);" //蓝色背景
        "color: rgb(255, 255, 255);" //白色字体
        "border-radius: 10px;"
        "border: 2px groove gray;"
        "border-style: outset;"
        "}"
        "QPushButton:hover{"
        "background-color: rgb(85, 170, 255);" //蓝色背景
        "}"
        "QPushButton:pressed{"
        "background-color: rgb(85, 255, 255);"
        "border-style: inset;"
        "}");
    m_serialPort = new QSerialPort(this);
    m_serialPort->setBaudRate(115200, QSerialPort::AllDirections);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setParity(QSerialPort::NoParity);
    QSerialPortInfo ports;
    for (const QSerialPortInfo &info: QSerialPortInfo::availablePorts()) {
        ui->serialList->addItem(info.portName());
        if (info.portName().contains("tty") && (info.portName().contains("USB") || info.portName().contains("usb")) ||
            info.portName().contains("ttySAC1")) {
            m_serialPort->setPortName(info.portName());
            m_serialPort->open(QSerialPort::ReadWrite);
        }
    }
    ui->IDset->setText(QString('0' + (int) deviceID));
    ui->FanSpeed->setMinimum(0);
    ui->FanSpeed->setMaximum(2);
    ui->FanSpeed->setSingleStep(1);
    auto *serialRefreshTimer = new QTimer(this);
    serialRefreshTimer->start(500);
    connect(serialRefreshTimer, &QTimer::timeout, this, &deviceControler::on_serialFresh);
    connect(ui->fanOnoff, &QPushButton::clicked, this, &deviceControler::on_fanOnoff);
    connect(ui->FanSpeed, &QDial::sliderMoved, this, &deviceControler::on_fanSpeed);
    connect(ui->serialList, &QListWidget::itemClicked, this, [=]() {
        m_serialPort->setPortName(ui->serialList->currentItem()->text());
        m_serialPort->open(QSerialPort::ReadWrite);
    });
    connect(ui->buzzContorl, &QPushButton::clicked, this, &deviceControler::on_buzzOnoff);
    connect(ui->hexOnoff, &QPushButton::clicked, this, &deviceControler::on_hexOnoff);
    connect(ui->ledOnoff, &QPushButton::clicked, this, &deviceControler::on_ledOnoff);
    connect(ui->exitButton, &QPushButton::clicked, this, &deviceControler::exit);
    connect(m_serialPort, &QSerialPort::readyRead, this, &deviceControler::on_serialReadable);
    connect(ui->IDset, &QLineEdit::editingFinished, this, [=] {
        deviceID = static_cast<uint8_t>(ui->IDset->text().toInt());
    });
    hexOn = true;
    hexToggle();
    alarmTimer = new QTimer;
    alarmTimer->setInterval(3000);
    connect(alarmTimer, &QTimer::timeout, this, [=] {
        uint8_t rawData[5] = {0xDD, 0, 0x24, 0x00, 0};
        rawData[1] = deviceID;
        rawData[4] = 0x03;
        m_serialPort->write(reinterpret_cast<const char *>(rawData), 5);

        m_serialPort->write(reinterpret_cast<const char *>(rawData), 5);
        alarmTimer->stop();
    });
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
    // uint8_t rawData[5] = {0xDD, 0, 0x24, 0x00, 0};
    // rawData[1] = deviceID;
    // rawData[4] = 0x04 + 0x04 * (deviceStatus.State.fan);
    // m_serialPort->write(reinterpret_cast<const char *>(rawData), 5);
    return true;
}

bool deviceControler::hexToggle() {
    // uint8_t rawData[5] = {0xDD, 0, 0x24, 0x00, 0};
    // rawData[1] = deviceID;
    // rawData[4] = 0x09 + hexOn;
    // hexOn = !hexOn;
    // m_serialPort->write(reinterpret_cast<const char *>(rawData), 5);
    return true;
}

bool deviceControler::lightToggle() {
    lightStatus = !lightStatus;
    uint64_t data;
    if (lightStatus) {
       data = 0x00FE2101010101;
    } else {
       data = 0x00FE2101010000;
    }
    m_serialPort->write(reinterpret_cast<const char *>(&data), 8);
    qDebug() << "send:" << lightStatus;
    return true;
}

bool deviceControler::setFanspeed(deviceControler::FanSpeed speed) {
    // uint8_t rawData[5] = {0xDD, 0, 0x24, 0x00, 0};
    // rawData[1] = deviceID;
    // rawData[4] = 0x05 + speed;
    // m_serialPort->write(reinterpret_cast<const char *>(rawData), 5);
    return true;
}

void deviceControler::on_hexOnoff() {
    hexToggle();
}

void deviceControler::on_ledOnoff() {
    lightToggle();
}

void deviceControler::on_fanSpeed() {
    auto newSpeed = static_cast<FanSpeed>(ui->FanSpeed->value());
    fanSpeed = newSpeed;
    setFanspeed(newSpeed);
}

void deviceControler::on_serialReadable() {
    unsigned char ref = static_cast<uint8_t>(m_serialPort->read(1)[0]);
    qDebug() << "head:" + ref;
    if (ref == 15) {
        QByteArray raw = m_serialPort->read(5);
        qDebug() << "recv:" + raw;
        // deviceStatus.tempVal = concat_bytes2(raw[3], raw[4]);
        uint8_t i = raw[0];
        deviceStatus.tempVal = i;
        i = raw[1];
        deviceStatus.humVal = i;
        deviceStatus.lightVal = (concat_bytes2(raw[2], raw[3]) - 4000) / 28763.0 * 100;
        emit deviceUpdate();
        // ui->fanStatus->setText(QString('0' + deviceStatus.State.fan));
        // ui->lightStatus->setText(QString('0' + !lightOn));
        // ui->buzzStatus->setText(QString('0' + buzzOn));
        // ui->hexStatus->setText(QString('0' + deviceStatus.State.hex));
    } else {
        qDebug() << "err recv:" + ref;
    }
    m_serialPort->readAll();
}

void deviceControler::exit() {
    this->close();
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
    auto name = m_serialPort->portName();
    if (name.isEmpty())
        name = "错误";
    return name;
}

uint8_t deviceControler::getDeviceId()  {
    if (m_serialPort->isOpen())
        return deviceID;
    else
        return 255;
}

void deviceControler::alarm_tigger() {
    if (alarmTimer->isActive())
        return;
    uint8_t rawData[5] = {0xDD, 0, 0x24, 0x00, 0};
    rawData[1] = deviceID;
    rawData[4] = 0x02;
    m_serialPort->write(reinterpret_cast<const char *>(rawData), 5);
    alarmTimer->start();
}

void deviceControler::on_buzzOnoff() {
    buzzToggle();
}

bool deviceControler::buzzToggle() {
    uint8_t rawData[5] = {0xDD, 0, 0x24, 0x00, 0};
    rawData[1] = deviceID;
    rawData[4] = 0x02 + buzzOn;
    buzzOn = !buzzOn;
    m_serialPort->write(reinterpret_cast<const char *>(rawData), 5);
    return true;
}
