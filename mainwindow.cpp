#include "mainwindow.h"
#include "authregist.h"
#include "jqchecksum.h"
#include "parse_data.h"
#include "settingdialog.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QModbusDataUnit>
#include <QModbusReply>
#include <QModbusRequest>
#include <QModbusRtuSerialMaster>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , timer(nullptr)
    , serial(nullptr)
    , K_timer(nullptr)
    , excel(nullptr) {
    ui->setupUi(this);

    timer = new QTimer(this);
    if (timer)
        connect(timer, SIGNAL(timeout()), this, SLOT(time_up()));

    K_timer = new QTimer(this);
    if (K_timer) {
        K_timer->setInterval(10);
        K_timer->setSingleShot(true);
        connect(K_timer, SIGNAL(timeout()), this, SLOT(key_time_out()));
    }

    m_settingsDialog = new settingDialog(this);
    connect(m_settingsDialog, SIGNAL(connect_button_pushed()), this, SLOT(on_connectButton_clicked()));
    serial = m_settingsDialog->serial();
    //连接槽，串口出现问题连接到错误处理函数
    connect(serial, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
        this, &MainWindow::handleError);
    ui->checkBox_ScanSwith->setEnabled(false);
    excel = new ExcelManger(this);
    auto auth = new AuthRegist(tr("age"));
}

MainWindow::~MainWindow() {
    if (serial) {
        if (serial->openMode() == QIODevice::ReadWrite) {
            serial->clear();
            serial->close();
        }
    }
    delete ui;
}

void MainWindow::handleError(QSerialPort::SerialPortError error) {
    qDebug() << "SerialPortError " << QString::number(error, 10);
    if (!serial)
        return;
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), serial->errorString());
        serial->close();
        ui->actionConnect->setEnabled(true);
        ui->actionDisconnect->setEnabled(false);
        ui->actiontest->setChecked(Qt::Unchecked);
    } else if (error == QSerialPort::PermissionError) {
        QMessageBox::critical(
            this, tr("Critical Error"),
            tr("\r\nThe device is not connect\r\nPlease Check"));
        if (serial->openMode() == QIODevice::ReadWrite) {
            serial->close();
        }
        ui->actionConnect->setEnabled(true);
        ui->actionDisconnect->setEnabled(false);
        ui->actiontest->setChecked(Qt::Unchecked);
    } else if (error == QSerialPort::NotOpenError) {
        QMessageBox::critical(
            this, tr("NotOpenError Error"),
            tr("\r\nThe serial is net opened\r\nPlease Check"));
        ui->actionConnect->setEnabled(true);
        ui->actionDisconnect->setEnabled(false);
        ui->actiontest->setChecked(Qt::Unchecked);
    }
}

void MainWindow::onStateChanged(int state) {
    if (state == QModbusDevice::UnconnectedState) {
        ui->actionConnect->setEnabled(true);
        ui->actionDisconnect->setEnabled(false);
    } else if (state == QModbusDevice::ConnectedState) {
        ui->actionConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        if (serial) {
            if (serial->openMode() == QIODevice::ReadWrite) {
                serial->clear();
                serial->close();
            }
        }
        this->close();
    } else {
        if ((event->isAutoRepeat()) ||
            (ui->checkBox_ScanSwith->checkState() == Qt::Unchecked))
            return;
        if (K_timer->isActive()) { //scan code event
            if (event->key() == Qt::Key_Return) {
                ui->lineEdit_writeDeviceName->setText(
                    ui->lineEdit_writeDeviceName->text() + currKey);
                qDebug() << currKey;
                ui->label_WriteState->setText(ui->lineEdit_writeDeviceName->text());
                ui->lineEdit_writeDeviceName->clear();
                qDebug() << tr("enter");
                on_writeButton_clicked();
                currKey.clear();
                K_timer->stop();
                return;

            } else {
                ui->lineEdit_writeDeviceName->setText(
                    ui->lineEdit_writeDeviceName->text() + currKey);
                // qDebug() << currKey;
            }
        }
        currKey = event->text();
        K_timer->start();
    }
}

void MainWindow::Read_Data() {
    //   timer->stop();
    ReceiveBuf += serial->readAll();

    timer->start(250);
}

void MainWindow::on_connectButton_clicked() {
    serial = m_settingsDialog->serial();
    if (!serial)
        return;
    if (serial->portName().length() == 0) {
        ui->statusBar->showMessage("Serial is empty", 3000);
        return;
    }
    if (serial->openMode() != QIODevice::ReadWrite) {
        if (serial->open(QIODevice::ReadWrite) == true) {
            serial->setBaudRate(
                m_settingsDialog->settings().baud);
            serial->setDataBits(
                m_settingsDialog->settings().dataBits);
            serial->setParity(
                m_settingsDialog->settings().parity);
            serial->setStopBits(
                m_settingsDialog->settings().stopBits);
            // serial->setFlowControl(QSerialPort::NoFlowControl);
            QObject::connect(serial, &QSerialPort::readyRead, this,
                &MainWindow::Read_Data);
            qDebug() << "open serial" << serial->portName() << "done";
            ui->actionConnect->setEnabled(false);
            ui->actionDisconnect->setEnabled(true);
            ui->actiontest->setChecked(Qt::Checked);
            ui->checkBox_ScanSwith->setEnabled(true);
        }
        statusBar()->showMessage(serial->portName().section("  ", 0, 0) + tr(" Connected"), 3000);
    } else {
        serial->clear();
        serial->close();
        ui->actionConnect->setEnabled(true);
        ui->actionDisconnect->setEnabled(false);
        ui->actiontest->setChecked(Qt::Unchecked);
        ui->checkBox_ScanSwith->setEnabled(false);
        statusBar()->showMessage(tr("Serial ") + tr("Disconnected"), 3000);
    }
}

void MainWindow::on_readButton_clicked() {

    if (!serial)
        return;
    if (serial->openMode() != QIODevice::ReadWrite)
        return;

    QByteArray ReadCmd("");
    ReadCmd.insert(ReadCmd.size(), 0x01);
    ReadCmd.insert(ReadCmd.size(), 0x03);
    ReadCmd.insert(ReadCmd.size(), static_cast<char>(264 >> 8));
    ReadCmd.insert(ReadCmd.size(), static_cast<char>(264 & 0x00ff));
    ReadCmd.insert(ReadCmd.size(), static_cast<char>(100 >> 8));
    ReadCmd.insert(ReadCmd.size(), static_cast<char>(100 & 0x00ff));

    auto crc16ForModbus = JQChecksum::crc16ForModbus(ReadCmd);
    qDebug() << "crc16ForModbus:" << crc16ForModbus << QString::number(crc16ForModbus, 16);
    ReadCmd.insert(ReadCmd.size(), static_cast<char>(crc16ForModbus & 0xff));
    ReadCmd.insert(ReadCmd.size(), static_cast<char>(crc16ForModbus >> 8));

    serial->write(ReadCmd);
    timer->start(1000);
    ReceiveBuf.clear();
}

void MainWindow::on_writeButton_clicked() {

    if (!serial)
        return;
    if (serial->openMode() != QIODevice::ReadWrite)
        return;
    Parse_Data write_data;
    if (ui->checkBox_ScanSwith->checkState() == Qt::Checked) {
        QString checkStr;
        checkStr = ui->comboBox_AirWater->currentText() + tr("-") +
            ui->comboBox_Model->currentText() + tr("-");
        qDebug() << checkStr;
        qDebug() << ui->label_WriteState->text();
        qDebug() << tr("x.lastIndexOf(y):")
                 << QString::number(ui->label_WriteState->text().lastIndexOf(checkStr), 10);
        if (ui->label_WriteState->text().lastIndexOf(checkStr) != 0) {
            return;
        }
        write_data.Parse_insert_flag(QString::number(REGISTER_FLAG, 16) /* tr("0x0505")*/);

        write_data.Parse_insert_deviceName(ui->label_WriteState->text());
    } else {
        write_data.Parse_insert_flag(QString::number(REGISTER_FLAG, 16) /* tr("0x0505")*/);
        write_data.Parse_insert_deviceName(ui->lineEdit_writeDeviceName->text());
        write_data.Parse_insert_productKey(ui->lineEdit_writeProductKey->text());
        write_data.Parse_insert_deviceSecret(ui->lineEdit_writeDeviceSecret->text());
        write_data.Parse_insert_clientID(ui->lineEdit_writeClientID->text());
    }
    QByteArray WriteCmd("");
    WriteCmd.insert(WriteCmd.size(), 0x01);
    WriteCmd.insert(WriteCmd.size(), 0x10);
    WriteCmd.insert(WriteCmd.size(), static_cast<char>(264 >> 8));
    WriteCmd.insert(WriteCmd.size(), static_cast<char>(264 & 0x00ff));
    WriteCmd.insert(WriteCmd.size(), static_cast<char>(100 >> 8));
    WriteCmd.insert(WriteCmd.size(), static_cast<char>(100 & 0x00ff));
    WriteCmd.insert(WriteCmd.size(), static_cast<char>(100 * 2));
    WriteCmd.append(write_data.GetArray());

    auto crc16ForModbus = JQChecksum::crc16ForModbus(WriteCmd);
    qDebug() << "crc16ForModbus:" << crc16ForModbus << QString::number(crc16ForModbus, 16);
    WriteCmd.insert(WriteCmd.length(), static_cast<char>(crc16ForModbus & 0xff));
    WriteCmd.insert(WriteCmd.length(), static_cast<char>(crc16ForModbus >> 8));
    QString stri(WriteCmd.toHex().toUpper());
    int len = stri.length() / 2;
    qDebug() << tr("QString count:") << QString::number(len, 10);
    for (int i = 1; i < len; i++) {
        stri.insert(2 * i + i - 1, " ");
    }
    qDebug() << stri;

    serial->write(WriteCmd);
    timer->start(1000);
    ReceiveBuf.clear();
}

void MainWindow::time_up() {
    if (ReceiveBuf.isEmpty())
        return;
    qDebug() << tr("get data:") << ReceiveBuf.toHex().toUpper();

    if (JQChecksum::crc16ForModbus(ReceiveBuf) != 0) {
        statusBar()->showMessage(tr("checksum error"), 3000);
        goto exit;
    }

    if (ReceiveBuf.at(0) != 1)
        goto exit;

    switch (ReceiveBuf.at(1)) {
        case QModbusPdu::ReadHoldingRegisters: //0x10
        {
            qDebug() << tr("data count:") << QString::number(static_cast<long>(static_cast<uchar>(ReceiveBuf.at(2))), 10);
            QByteArray data = ReceiveBuf.mid(3, static_cast<uchar>(ReceiveBuf.at(2)));
            QString stri(data.toHex().toUpper());
            int len = stri.length() / 2;
            qDebug() << tr("QString count:") << QString::number(len, 10);
            for (int i = 1; i < len; i++) {
                stri.insert(2 * i + i - 1, " ");
            }
            qDebug() << stri;

            Parse_Data P_data;
            P_data.Parse_Read(data);

            ui->lineEdit_readFlag->setText(QString::number(P_data.info.flag, 16).toUpper());
            ui->lineEdit_readDeviceName->setText(QString::fromLocal8Bit(
                P_data.info.device_name, static_cast<int>(strlen(P_data.info.device_name))));
            ui->lineEdit_readProductKey->setText(QString::fromLocal8Bit(
                P_data.info.product_key, static_cast<int>(strlen(P_data.info.product_key))));
            ui->lineEdit_readDeviceSecret->setText(QString::fromLocal8Bit(
                P_data.info.device_secret, static_cast<int>(strlen(P_data.info.device_secret))));
            ui->lineEdit_readClientID->setText(QString::fromLocal8Bit(
                P_data.info.device_id, static_cast<int>(strlen(P_data.info.device_id))));
            statusBar()->showMessage(tr("read sucess"), 3000);
            break;
        }

        case QModbusPdu::WriteMultipleRegisters: {
            statusBar()->showMessage(tr("write sucess"), 3000);
            break;
        }
        default:
            statusBar()->showMessage(tr("function code error"), 3000);
            break;
    }
exit:
    timer->stop();
    ReceiveBuf.clear();
}

void MainWindow::on_checkBox_ScanSwith_stateChanged(int arg1) {
    if (arg1 == Qt::Checked) {
        ui->lineEdit_writeDeviceName->setReadOnly(true);
        ui->lineEdit_writeProductKey->setReadOnly(true);
        ui->lineEdit_writeDeviceSecret->setReadOnly(true);
        ui->lineEdit_writeClientID->setReadOnly(true);
        ui->lineEdit_writeDeviceName->clear();
        ui->lineEdit_writeProductKey->clear();
        ui->lineEdit_writeDeviceSecret->clear();
        ui->lineEdit_writeClientID->clear();
        ui->comboBox_Model->setEnabled(false);
        ui->readButton->setEnabled(false);
        ui->writeButton->setEnabled(false);
        ui->actionConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(false);
        ui->actionOptions->setEnabled(false);
        ui->lineEdit_writeDeviceName->grabKeyboard();
    } else {
        ui->lineEdit_writeDeviceName->setReadOnly(false);
        ui->lineEdit_writeProductKey->setReadOnly(false);
        ui->lineEdit_writeDeviceSecret->setReadOnly(false);
        ui->lineEdit_writeClientID->setReadOnly(false);
        ui->comboBox_Model->setEnabled(true);
        ui->readButton->setEnabled(true);
        ui->writeButton->setEnabled(true);
        ui->actionConnect->setEnabled(true);
        ui->actionDisconnect->setEnabled(true);
        ui->actionOptions->setEnabled(true);
        ui->lineEdit_writeDeviceName->releaseKeyboard();
    }
}

void MainWindow::key_time_out() {
    qDebug() << currKey;
    ui->lineEdit_writeDeviceName->clear();
}

void MainWindow::on_pushButton_clicked() {

    if (excel->input_check(ui->lineEdit_writeDeviceName->text()) == 0) {
        qDebug() << ui->lineEdit_writeDeviceName->text() << tr("already written");
        return;
    }
    excel->input_sid(ui->lineEdit_writeDeviceName->text());
}

void MainWindow::on_actionOptions_triggered() {
    if (m_settingsDialog)
        m_settingsDialog->show();
}

void MainWindow::on_actionConnect_triggered() {
    if (serial->openMode() != QIODevice::ReadWrite) {
        on_connectButton_clicked();
    }
    return;
}

void MainWindow::on_actionDisconnect_triggered() {
    if (serial->openMode() != QIODevice::ReadWrite) {
        return;
    }
    on_connectButton_clicked();
}
