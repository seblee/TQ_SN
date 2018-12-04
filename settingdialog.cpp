#include "settingdialog.h"
#include "mainwindow.h"
#include "ui_settingdialog.h"
#include <QDebug>
#include <QSerialPortInfo>

settingDialog::settingDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::settingDialog)
    , m_serial(nullptr) {
    ui->setupUi(this);

    ui->parityCombo->setCurrentIndex(0);
    ui->baudCombo->setCurrentText(QString::number(m_settings.baud));
    ui->dataBitsCombo->setCurrentText(QString::number(m_settings.dataBits));
    ui->stopBitsCombo->setCurrentText(QString::number(m_settings.stopBits));
    //    ui->timeoutSpinner->setValue(m_settings.responseTime);
    //    ui->retriesSpinner->setValue(m_settings.numberOfRetries);

    m_serial = new QSerialPort(this);
}

settingDialog::~settingDialog() {
    delete ui;
}

settingDialog::Settings settingDialog::settings() const {
    return m_settings;
}

QSerialPort *settingDialog::serial() const {
    return m_serial;
}

void settingDialog::on_applyButton_clicked() {
    m_settings.parity = QSerialPort::Parity(ui->parityCombo->currentIndex());
    if (m_settings.parity > 0)
        m_settings.parity = QSerialPort::Parity(m_settings.parity + 1);
    m_settings.baud = QSerialPort::BaudRate(ui->baudCombo->currentText().toInt());
    m_settings.dataBits = QSerialPort::DataBits(ui->dataBitsCombo->currentText().toInt());
    m_settings.stopBits = QSerialPort::StopBits(ui->stopBitsCombo->currentText().toInt());
    //    m_settings.responseTime = ui->timeoutSpinner->value();
    //    m_settings.numberOfRetries = ui->retriesSpinner->value();

    QString str = ui->Serial_item->currentText();
    if (m_serial->openMode() != QIODevice::ReadWrite) {
        if (str.length() == 0) {
            return;
        }
        m_serial->setPortName(str.section("  ", 0, 0));
        emit connect_button_pushed();
    }
    hide();
}

void settingDialog::on_pushButton_refresh_clicked() {
    ui->Serial_item->clear();
    foreach (const QSerialPortInfo &com_info, QSerialPortInfo::availablePorts()) {
        qDebug() << com_info.portName() << com_info.description();
        ui->Serial_item->addItem(com_info.portName() + QWidget::tr("   ") +
            com_info.description());
    }
}
