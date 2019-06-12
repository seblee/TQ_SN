#include "mainwindow.h"
#include "authregist.h"
#include "jqchecksum.h"
#include "parse_data.h"
#include "settingdialog.h"
#include "ui_mainwindow.h"
#include <QJsonDocument>
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
    , Connect_timer(nullptr)
    , serial(nullptr)
    , K_timer(nullptr)
    , m_settingsDialog(nullptr)
    , excel(nullptr) {
    ui->setupUi(this);
    Retries = 0;
    timer = new QTimer(this);
    Connect_timer = new QTimer(this);
    if (timer) {
        connect(timer, SIGNAL(timeout()), this, SLOT(time_up()));
        Connect_timer->setSingleShot(true);
    }

    K_timer = new QTimer(this);
    if (K_timer) {
        K_timer->setInterval(50);
        K_timer->setSingleShot(true);
        connect(K_timer, SIGNAL(timeout()), this, SLOT(key_time_out()));
    }

    m_settingsDialog = new settingDialog(this);
    connect(m_settingsDialog, &settingDialog::connect_button_pushed, this, &MainWindow::connectButton_clicked);

    serial = m_settingsDialog->serial();

    Retries = m_settingsDialog->settings().numberOfRetries;

    if (Connect_timer) {
        Connect_timer->setInterval(m_settingsDialog->settings().responseTime);
        Connect_timer->setSingleShot(false);
        connect(Connect_timer, &QTimer::timeout, this, &MainWindow::checkout_connect);
        Connect_timer->start();
    }

    //连接槽，串口出现问题连接到错误处理函数
    connect(serial, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
        this, &MainWindow::handleError);
    ui->checkBox_ScanSwith->setEnabled(false);
    excel = new ExcelManger(this);

    checkWrite = false;
    needtowrite = false;
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
                ui->label_WriteState->setText(strCache);
                strCache.clear();

                QString checkStr = ui->comboBox_AirWater->currentText() + tr("-") +
                    ui->comboBox_Model->currentText() + tr("-");
                qDebug() << checkStr;
                qDebug() << ui->label_WriteState->text();
                qDebug() << tr("x.lastIndexOf(y):")
                         << QString::number(ui->label_WriteState->text().lastIndexOf(checkStr), 10);
                if (ui->label_WriteState->text().size()>0/* ui->label_WriteState->text().lastIndexOf(checkStr) == 0*/) {
                    ui->OK_checkBox->setCheckState(Qt::Unchecked);
                    ui->OK_checkBox->setText(tr(""));

                    if (excel->input_check(ui->label_WriteState->text()) == 0) {
                        qDebug() << ui->label_WriteState->text() << tr("already written");
                        disconnect(excel, &ExcelManger::json_back, nullptr, nullptr);
                        connect(excel, &ExcelManger::json_back, this, &MainWindow::write_to_file);
                        excel->output_sid(ui->label_WriteState->text());
                    } else {
                        statusBar()->showMessage(tr("not find in excel"), 3000);
                        auto auth = new AuthRegist(ui->label_WriteState->text(), ui->comboBox_AirWater->currentText());
                        connect(auth, &AuthRegist::register_back, this, &MainWindow::write_to_file);
                        auth->start_request();
                    }
                } else {
                    statusBar()->showMessage(tr("序列号错误"), 3000);
                }

                qDebug() << tr("enter");

                K_timer->stop();
                return;

            } else {
                strCache.append(event->text());
            }
        }else {
            strCache.clear();
            strCache.append(event->text());
        }
        K_timer->start();
    }
}

void MainWindow::send_to_bord(QString deviceName, QString productKey, QString deviceSecret) {
    if (!serial)
        return;
    if (serial->openMode() != QIODevice::ReadWrite)
        return;

    Parse_Data write_data;

    QString checkStr;
    checkStr = ui->comboBox_AirWater->currentText() + tr("-") +
        ui->comboBox_Model->currentText() + tr("-");
    qDebug() << checkStr;
    qDebug() << ui->label_WriteState->text();
    qDebug() << tr("x.lastIndexOf(y):")
             << QString::number(deviceName.lastIndexOf(checkStr), 10);
    if (deviceName.size()==0/* deviceName.lastIndexOf(checkStr) != 0*/) {
        return;
    }

    write_data.Parse_insert_flag(QString::number(REGISTER_FLAG, 16));
    write_data.Parse_insert_deviceName(deviceName);
    write_data.Parse_insert_productKey(productKey);
    write_data.Parse_insert_deviceSecret(deviceSecret);
    write_data.Parse_insert_clientID(deviceName);

    readFlag.clear();
    device_name.clear();
    product_key.clear();
    device_secret.clear();
    device_id.clear();

    readFlag.append(QString::number(REGISTER_FLAG, 16).toUpper());
    device_name.append(deviceName);
    product_key.append(productKey);
    device_secret.append(deviceSecret);
    device_id.append(deviceName);

    QByteArray WriteCmd("");
    WriteCmd.insert(WriteCmd.size(), 0x01);
    WriteCmd.insert(WriteCmd.size(), 0x10);
    WriteCmd.insert(WriteCmd.size(), static_cast<char>(Modbus_WRITE_ADD >> 8));
    WriteCmd.insert(WriteCmd.size(), static_cast<char>(Modbus_WRITE_ADD & 0x00ff));
    WriteCmd.insert(WriteCmd.size(), static_cast<char>(100 >> 8));
    WriteCmd.insert(WriteCmd.size(), static_cast<char>(100 & 0x00ff));
    WriteCmd.insert(WriteCmd.size(), static_cast<char>(100 * 2));
    WriteCmd.append(write_data.GetArray());

    auto crc16ForModbus = JQChecksum::crc16ForModbus(WriteCmd);
    //  qDebug()  Q << "crc16ForModbus:" << crc16ForModbus << QString::number(crc16ForModbus, 16);
    WriteCmd.insert(WriteCmd.length(), static_cast<char>(crc16ForModbus & 0xff));
    WriteCmd.insert(WriteCmd.length(), static_cast<char>(crc16ForModbus >> 8));

    serial->write(WriteCmd);
    timer->start(2000);
    ReceiveBuf.clear();
    ui->OK_checkBox->setCheckState(Qt::Unchecked);
    ui->OK_checkBox->setText(tr("正在写入"));
    checkWrite = true;
}

void MainWindow::Read_Data() {
    //   timer->stop();
    ReceiveBuf += serial->readAll();

    timer->start(250);
}

void MainWindow::connectButton_clicked() {
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
            QObject::connect(serial, &QSerialPort::readyRead, this,
                &MainWindow::Read_Data);
            qDebug() << "open serial" << serial->portName() << "done";
            ui->actionConnect->setEnabled(false);
            ui->actionDisconnect->setEnabled(true);
            ui->actiontest->setChecked(Qt::Checked);
            ui->checkBox_ScanSwith->setEnabled(true);
            qDebug() << "open serial" << QString::number(m_settingsDialog->settings().responseTime, 10);

            Connect_timer->setInterval(m_settingsDialog->settings().responseTime);
            Connect_timer->setSingleShot(false);
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
    if (timer->isActive()) {
        qDebug() << tr("timer->isActive()");
        return;
    }

    QByteArray ReadCmd("");
    ReadCmd.insert(ReadCmd.size(), 0x01);
    ReadCmd.insert(ReadCmd.size(), 0x03);
    ReadCmd.insert(ReadCmd.size(), static_cast<char>(Modbus_READ_ADD >> 8));
    ReadCmd.insert(ReadCmd.size(), static_cast<char>(Modbus_READ_ADD & 0x00ff));
    ReadCmd.insert(ReadCmd.size(), static_cast<char>(100 >> 8));
    ReadCmd.insert(ReadCmd.size(), static_cast<char>(100 & 0x00ff));

    auto crc16ForModbus = JQChecksum::crc16ForModbus(ReadCmd);
    //    qDebug() << "crc16ForModbus:" << crc16ForModbus << QString::number(crc16ForModbus, 16);
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
    device_name.clear();
    product_key.clear();
    device_secret.clear();

    ui->lineEdit_writeProductKey->clear();
    ui->lineEdit_writeDeviceSecret->clear();

    QString checkStr = ui->comboBox_AirWater->currentText() + tr("-") +
        ui->comboBox_Model->currentText() + tr("-");
    qDebug() << checkStr;
    qDebug() << ui->lineEdit_writeDeviceName->text();
    qDebug() << tr("x.lastIndexOf(y):")
             << QString::number(ui->lineEdit_writeDeviceName->text().lastIndexOf(checkStr), 10);
    if (ui->lineEdit_writeDeviceName->text().size() > 0/*ui->lineEdit_writeDeviceName->text().lastIndexOf(checkStr) == 0*/) {
        ui->OK_checkBox->setCheckState(Qt::Unchecked);
        ui->OK_checkBox->setText(tr(""));
        if (excel->input_check(ui->lineEdit_writeDeviceName->text()) == 0) {
            qDebug() << ui->lineEdit_writeDeviceName->text() << tr("already written");
            disconnect(excel, &ExcelManger::json_back, nullptr, nullptr);
            connect(excel, &ExcelManger::json_back, this, &MainWindow::write_to_file);
            excel->output_sid(ui->lineEdit_writeDeviceName->text());
        } else {
            statusBar()->showMessage(tr("not find in excel"), 3000);
            auto auth = new AuthRegist(ui->lineEdit_writeDeviceName->text(), ui->comboBox_AirWater->currentText());
            connect(auth, &AuthRegist::register_back, this, &MainWindow::write_to_file);
            auth->start_request();
        }
    } else {
        statusBar()->showMessage(tr("序列号错误/为空"), 3000);
    }
}

void MainWindow::time_up() {
    if (ReceiveBuf.isEmpty()) {
        if (checkWrite) {
            ui->OK_checkBox->setText(tr("写入超时"));
            checkWrite = false;
        }
        goto exit;
    }
    // qDebug() << tr("get data:") << ReceiveBuf.toHex().toUpper();

    if (JQChecksum::crc16ForModbus(ReceiveBuf) != 0) {
        statusBar()->showMessage(tr("checksum error"), 3000);
        goto exit;
    }

    if (ReceiveBuf.at(0) != 1)
        goto exit;

    switch (ReceiveBuf.at(1)) {
        case QModbusPdu::ReadHoldingRegisters: //0x10
        {
            Retries = 0;
            QByteArray data = ReceiveBuf.mid(3, static_cast<uchar>(ReceiveBuf.at(2)));
            QString stri(data.toHex().toUpper());

            Parse_Data P_data;
            P_data.Parse_Read(data);
            if (ui->lineEdit_readFlag->text() != QString::number(P_data.info.flag, 16).toUpper())
                ui->lineEdit_readFlag->setText(QString::number(P_data.info.flag, 16).toUpper());

            if (ui->lineEdit_readDeviceName->text() !=
                QString::fromLocal8Bit(P_data.info.device_name, static_cast<int>(strlen(P_data.info.device_name))))
                ui->lineEdit_readDeviceName->setText(
                    QString::fromLocal8Bit(P_data.info.device_name, static_cast<int>(strlen(P_data.info.device_name))));

            if (ui->lineEdit_readProductKey->text() !=
                QString::fromLocal8Bit(P_data.info.product_key, static_cast<int>(strlen(P_data.info.product_key))))
                ui->lineEdit_readProductKey->setText(
                    QString::fromLocal8Bit(P_data.info.product_key, static_cast<int>(strlen(P_data.info.product_key))));

            if (ui->lineEdit_readDeviceSecret->text() !=
                QString::fromLocal8Bit(P_data.info.device_secret, static_cast<int>(strlen(P_data.info.device_secret))))
                ui->lineEdit_readDeviceSecret->setText(
                    QString::fromLocal8Bit(P_data.info.device_secret, static_cast<int>(strlen(P_data.info.device_secret))));

            if (ui->lineEdit_readClientID->text() !=
                QString::fromLocal8Bit(P_data.info.device_id, static_cast<int>(strlen(P_data.info.device_id))))
                ui->lineEdit_readClientID->setText(
                    QString::fromLocal8Bit(P_data.info.device_id, static_cast<int>(strlen(P_data.info.device_id))));

            if (checkWrite) {
                checkWrite = false;
                statusBar()->showMessage(tr("read sucess"), 3000);
                if (readFlag != ui->lineEdit_readFlag->text()) {
                    ui->OK_checkBox->setCheckState(Qt::PartiallyChecked);
                    ui->OK_checkBox->setText(tr("写入失败"));
                    break;
                }
                if (device_name != ui->lineEdit_readDeviceName->text()) {
                    ui->OK_checkBox->setCheckState(Qt::PartiallyChecked);
                    ui->OK_checkBox->setText(tr("写入失败"));
                    break;
                }
                if (product_key != ui->lineEdit_readProductKey->text()) {
                    ui->OK_checkBox->setCheckState(Qt::PartiallyChecked);
                    ui->OK_checkBox->setText(tr("写入失败"));
                    break;
                }
                if (device_secret != ui->lineEdit_readDeviceSecret->text()) {
                    ui->OK_checkBox->setCheckState(Qt::PartiallyChecked);
                    ui->OK_checkBox->setText(tr("写入失败"));
                    break;
                }
                if (device_id != ui->lineEdit_readClientID->text()) {
                    ui->OK_checkBox->setCheckState(Qt::PartiallyChecked);
                    ui->OK_checkBox->setText(tr("写入失败"));
                    break;
                }
                ui->OK_checkBox->setCheckState(Qt::Checked);
                ui->OK_checkBox->setText(tr("写入成功"));
            }

            break;
        }

        case QModbusPdu::WriteMultipleRegisters: {
            Retries = 0;
            statusBar()->showMessage(tr("write sucess"), 3000);
            checkWrite = true;
            ui->OK_checkBox->setCheckState(Qt::Unchecked);
            ui->OK_checkBox->setText(tr("正在读取"));
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

        ui->lineEdit_writeDeviceName->clear();
        ui->lineEdit_writeProductKey->clear();
        ui->lineEdit_writeDeviceSecret->clear();

        ui->comboBox_Model->setEnabled(false);
        ui->writeButton->setEnabled(false);
        ui->actionConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(false);
        ui->actionOptions->setEnabled(false);
        ui->lineEdit_writeDeviceName->grabKeyboard();
    } else {
        ui->lineEdit_writeDeviceName->setReadOnly(false);
        ui->lineEdit_writeProductKey->setReadOnly(false);
        ui->lineEdit_writeDeviceSecret->setReadOnly(false);

        ui->comboBox_Model->setEnabled(true);
        ui->writeButton->setEnabled(true);
        ui->actionDisconnect->setEnabled(true);
        ui->actionOptions->setEnabled(true);
        ui->lineEdit_writeDeviceName->releaseKeyboard();
    }
}

void MainWindow::key_time_out() {
    strCache.clear();
    ui->lineEdit_writeDeviceName->clear();
}

void MainWindow::on_pushButton_clicked() {

    if (excel->input_check(ui->lineEdit_writeDeviceName->text()) == 0) {
        qDebug() << ui->lineEdit_writeDeviceName->text() << tr("already written");
        disconnect(excel, &ExcelManger::json_back, nullptr, nullptr);
        connect(excel, &ExcelManger::json_back, this, &MainWindow::write_to_file);
        excel->output_sid(ui->lineEdit_writeDeviceName->text());
    } else {
        auto auth = new AuthRegist(tr("register_device_test_04"), ui->comboBox_AirWater->currentText());
        connect(auth, &AuthRegist::register_back, this, &MainWindow::write_to_file);
        auth->start_request();
    }
}

void MainWindow::on_actionOptions_triggered() {
    if (m_settingsDialog)
        m_settingsDialog->show();
}

void MainWindow::on_actionConnect_triggered() {
    if (serial->openMode() != QIODevice::ReadWrite) {
        connectButton_clicked();
    }
    return;
}

void MainWindow::on_actionDisconnect_triggered() {
    if (serial->openMode() != QIODevice::ReadWrite) {
        return;
    }
    connectButton_clicked();
}

void MainWindow::write_to_file(QJsonDocument data) {
    if (data.isNull() || data.isEmpty())
        return;
    if (data.isObject()) {
        QJsonObject object = data.object();

        if (object.contains("code")) {
            QJsonValue value = object.value("code");
            qDebug() << tr("code:%1").arg(value.toVariant().toInt()) << object.value("message").toString();

            if (value.isDouble()) {
                device_name.clear();
                product_key.clear();
                device_secret.clear();
                if ((value.toVariant().toInt() == 200) || (value.toVariant().toInt() == 100)) {
                    QJsonObject dataOBJ = object.value("data").toObject();
                    device_name.append(dataOBJ.value("deviceName").toString());
                    product_key.append(dataOBJ.value("productKey").toString());
                    device_secret.append(dataOBJ.value("deviceSecret").toString());

                    ui->lineEdit_writeDeviceName->setText(device_name);
                    ui->lineEdit_writeProductKey->setText(product_key);
                    ui->lineEdit_writeDeviceSecret->setText(device_secret);

                    if (value.toVariant().toInt() == 200)
                        excel->input_sid(device_name, product_key, device_secret);

                    needtowrite = true;
                } else {
                    QMessageBox::critical(this, tr("SID获取失败"), tr("code:%1").arg(value.toVariant().toInt()) + tr(" ") + object.value("message").toString());
                }
            }
        } else if (object.contains("error")) {
            QJsonValue value = object.value("error");
            qDebug() << value.toString();
        }
    }
}

void MainWindow::checkout_connect() {
    Retries++;
    if (Retries <= m_settingsDialog->settings().numberOfRetries) {
        ui->actionDeviceOnline->setChecked(Qt::Checked);
    } else {
        ui->actionDeviceOnline->setChecked(Qt::Unchecked);
    }
    if (needtowrite) {
        needtowrite = false;
        send_to_bord(device_name, product_key, device_secret);
    } else
        on_readButton_clicked();
}
