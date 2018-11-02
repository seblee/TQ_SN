#include "mainwindow.h"
#include "jqchecksum.h"
#include "parse_data.h"
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
#ifdef CHANAL_SERIAL
    , serial(nullptr)
#elif defined CHANAL_MODBUS
    , modbusDevice(nullptr)
#endif
    , K_timer(nullptr) {
    //    serial = new QSerialPort;

    ui->setupUi(this);
#ifdef CHANAL_SERIAL
    serial = new QSerialPort(this);
#elif defined CHANAL_MODBUS
    modbusDevice = new QModbusRtuSerialMaster(this);
    connect(modbusDevice, &QModbusClient::errorOccurred,
        [this](QModbusDevice::Error) {
            statusBar()->showMessage(modbusDevice->errorString(), 5000);
        });
    if (!modbusDevice) {
        ui->connectButton->setDisabled(true);

        statusBar()->showMessage(tr("Could not create Modbus master."), 5000);
    } else {
        connect(modbusDevice, &QModbusClient::stateChanged, this,
            &MainWindow::onStateChanged);
    }
#endif
    timer = new QTimer(this);
    if (timer)
        connect(timer, SIGNAL(timeout()), this, SLOT(time_up()));

    K_timer = new QTimer(this);
    if (K_timer) {
        K_timer->setInterval(10);
        K_timer->setSingleShot(true);
        connect(K_timer, SIGNAL(timeout()), this, SLOT(key_time_out()));
    }
}

MainWindow::~MainWindow() {
#ifdef CHANAL_SERIAL
    if (serial) {
        serial->clear();
        serial->close();
    }
#elif defined CHANAL_MODBUS
    if (modbusDevice)
        if (modbusDevice->state() == QModbusDevice::ConnectedState)
            modbusDevice->disconnectDevice();
#endif
    delete ui;
}

void MainWindow::handleError(QSerialPort::SerialPortError error) {
    qDebug() << "SerialPortError " << QString::number(error, 10);
    if (!serial)
        return;
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), serial->errorString());
        serial->clear();
        serial->close();
        ui->connectButton->setText(tr("Connect"));
        ui->CheckBox_SerialStatus->setCheckState(Qt::Unchecked);
    } else if (error == QSerialPort::PermissionError) {
        QMessageBox::critical(
            this, tr("Critical Error"),
            tr("\r\nThe serial has been already opened\r\nPlease Check"));
        serial->clear();
        serial->close();
        ui->connectButton->setText(tr("Connect"));
        ui->CheckBox_SerialStatus->setCheckState(Qt::Unchecked);
    }
}

void MainWindow::onStateChanged(int state) {
    if (state == QModbusDevice::UnconnectedState)
        ui->connectButton->setText(tr("Connect"));
    else if (state == QModbusDevice::ConnectedState)
        ui->connectButton->setText(tr("Disconnect"));
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
#ifdef CHANAL_SERIAL
        if (serial) {
            serial->clear();
            serial->close();
        }
#elif defined CHANAL_MODBUS
        if (modbusDevice)
            if (modbusDevice->state() == QModbusDevice::ConnectedState)
                modbusDevice->disconnectDevice();
#endif
        this->close();
    } else {
        if ((event->isAutoRepeat()) ||
            (ui->checkBox_ScanSwith->checkState() == Qt::Unchecked))
            return;
        if (K_timer->isActive()) { //scan code event
            if (event->key() == Qt::Key_Return) {
                ui->label_WriteState->setText(ui->lineEdit_writeDeviceName->text());
                ui->lineEdit_writeDeviceName->clear();
                qDebug() << tr("enter");
                currKey.clear();
                K_timer->stop();
                return;

            } else {
                ui->lineEdit_writeDeviceName->setText(
                    ui->lineEdit_writeDeviceName->text() + event->text());
                qDebug() << event->text();
            }
        }
        currKey = event->text();
        K_timer->start();
    }
}
void MainWindow::on_pushButton_2_clicked() {
    ui->Serial_item->clear();
    foreach (const QSerialPortInfo &com_info, QSerialPortInfo::availablePorts()) {
        qDebug() << com_info.portName() << com_info.description();
        ui->Serial_item->addItem(com_info.portName() + QWidget::tr("   ") +
            com_info.description());
    }
    foreach (const QSerialPortInfo &com_info, QSerialPortInfo::availablePorts()) {
        qDebug() << com_info.portName() << com_info.description();
        ui->Serial_item->addItem(com_info.portName() + QWidget::tr("   ") +
            com_info.description());
    }
}
#ifdef CHANAL_SERIAL
void MainWindow::Read_Data() {
    //   timer->stop();
    ReceiveBuf += serial->readAll();

    if (!ReceiveBuf.isEmpty())
        qDebug() << tr("get data:") << ReceiveBuf.toHex();

    timer->start(250);
    //  connect(timer, SIGNAL(timeout()), this, SLOT(time_up()));
}
#elif defined CHANAL_MODBUS
void MainWindow::readReady() {
    QModbusReply *reply = qobject_cast<QModbusReply *>(sender());

    if (!reply)
        return;
    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        static QByteArray data;
        data.resize(static_cast<int>(unit.valueCount()) * 2);

        for (uint i = 0; i < unit.valueCount(); i++) {
            data.insert(static_cast<int>(i * 2),
                static_cast<char>(unit.value(static_cast<int>(i)) >> 8));
            data.insert(static_cast<int>(i * 2 + 1),
                static_cast<char>(unit.value(static_cast<int>(i)) & 0x00ff));
        }
        QString stri(data.toHex().toUpper());
        int len = stri.length() / 2;

        for (int i = 1; i < len; i++) {
            stri.insert(2 * i + i - 1, " ");
        }
        qDebug() << stri;

        Parse_Data P_data;
        P_data.Parse_Read(data);
        ui->lineEdit_readFlag->setText(tr("0x") +
            QString::number(P_data.info.flag, 16));
        ui->lineEdit_readDeviceName->setText(QString::fromLocal8Bit(
            P_data.info.device_name, sizeof(P_data.info.device_name)));
        ui->lineEdit_readProductKey->setText(QString::fromLocal8Bit(
            P_data.info.product_key, sizeof(P_data.info.product_key)));
        ui->lineEdit_readDeviceSecret->setText(QString::fromLocal8Bit(
            P_data.info.device_secret, sizeof(P_data.info.device_secret)));
        ui->lineEdit_readClientID->setText(QString::fromLocal8Bit(
            P_data.info.device_id, sizeof(P_data.info.device_id)));
    } else if (reply->error() == QModbusDevice::ProtocolError) {
        statusBar()->showMessage(tr("Read response error:%1(Modbus exception:0x%2)")
                                     .arg(reply->errorString())
                                     .arg(reply->error(), -1, 16),
            5000);
    } else {
        statusBar()->showMessage(
            tr("Read response error:%1(code:0x%2)")
                .arg(reply->errorString().arg(reply->error(), -1, 16), 5000));
    }
    reply->deleteLater();
}

#endif

void MainWindow::on_connectButton_clicked() {
#ifdef CHANAL_SERIAL
    QString str = ui->Serial_item->currentText();
    if (!serial)
        return;
    statusBar()->clearMessage();
    if (serial->openMode() != QIODevice::ReadWrite) {
        if (str.length() == 0) {
            ui->statusBar->showMessage("Serial is empty", 3000);
            return;
        }
        connect(serial, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
            this, &MainWindow::handleError);
        //连接槽，串口出现问题连接到错误处理函数
        serial->setPortName(str.section("  ", 0, 0));
        if (serial->open(QIODevice::ReadWrite) == true) {
            serial->setBaudRate(QSerialPort::Baud19200);
            serial->setDataBits(QSerialPort::Data8);
            serial->setParity(QSerialPort::NoParity);
            serial->setStopBits(QSerialPort::OneStop);
            QObject::connect(serial, &QSerialPort::readyRead, this,
                &MainWindow::Read_Data);

            qDebug() << "open serial" << serial->portName() << "done";
            ui->connectButton->setText(tr("Disconnect"));
            ui->CheckBox_SerialStatus->setCheckState(Qt::Checked);
        }
        statusBar()->showMessage(str.section("  ", 0, 0) + tr(" Connected"));
    } else {
        serial->clearError();
        serial->clear();
        serial->close();
        ui->connectButton->setText(tr("Connect"));
        ui->CheckBox_SerialStatus->setCheckState(Qt::Unchecked);
        statusBar()->showMessage(tr("Serial ") + tr("Disconnected"));
    }
#elif defined CHANAL_MODBUS

    QString str = ui->Serial_item->currentText();
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();
    if (modbusDevice->state() != QModbusDevice::ConnectedState) {
        if (str.length() == 0) {
            ui->statusBar->showMessage("Serial is empty", 3000);
            return;
        }
        modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter,
            str.section("  ", 0, 0));
        modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter,
            QSerialPort::NoParity);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,
            QSerialPort::Baud19200);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,
            QSerialPort::Data8);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,
            QSerialPort::OneStop);
        modbusDevice->setTimeout(1000);
        modbusDevice->setNumberOfRetries(0);
        if (!modbusDevice->connectDevice()) {
            statusBar()->showMessage(tr("Connect failed: ") +
                modbusDevice->errorString());
        } else {
            statusBar()->showMessage(tr("Connect sucess"));
            modbusDevice->setParent(serial);
            serial->bytesAvailable();
        }
    } else {
        modbusDevice->disconnectDevice();
    }
#endif
}

void MainWindow::on_readButton_clicked() {
#ifdef CHANAL_SERIAL
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

#elif defined CHANAL_MODBUS

    if (!modbusDevice)
        return;
    if (modbusDevice->state() != QModbusDevice::ConnectedState)
        return;
    statusBar()->clearMessage();
    QModbusDataUnit readUnit(QModbusDataUnit::HoldingRegisters, 264, 100);
    if (QModbusReply *reply = modbusDevice->sendReadRequest(readUnit, 1)) {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, &MainWindow::readReady);
        } else {
            delete reply;
        }
    } else {
        statusBar()->showMessage(tr("Read error:") + modbusDevice->errorString());
    }
#endif
}

void MainWindow::on_writeButton_clicked() {

#ifdef CHANAL_SERIAL
    if (!serial)
        return;
    if (serial->openMode() != QIODevice::ReadWrite)
        return;
    Parse_Data write_data;
    write_data.Parse_insert_flag(tr("0x0505"));
    write_data.Parse_insert_deviceName(ui->lineEdit_writeDeviceName->text());
    write_data.Parse_insert_productKey(ui->lineEdit_writeProductKey->text());
    write_data.Parse_insert_deviceSecret(ui->lineEdit_writeDeviceSecret->text());
    write_data.Parse_insert_clientID(ui->lineEdit_writeClientID->text());
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

#elif defined CHANAL_MODBUS
    if (!modbusDevice)
        return;
    statusBar()->clearMessage(); //清除状态栏显示的信息
    Parse_Data write_data;
    write_data.Parse_insert_flag(tr("0x0505"));
    write_data.Parse_insert_deviceName(ui->lineEdit_writeDeviceName->text());
    write_data.Parse_insert_productKey(ui->lineEdit_writeProductKey->text());
    write_data.Parse_insert_deviceSecret(ui->lineEdit_writeDeviceSecret->text());
    write_data.Parse_insert_clientID(ui->lineEdit_writeClientID->text());

    QModbusDataUnit writeunit(QModbusDataUnit::HoldingRegisters, 264, 100);
    QString strDis = write_data.GetArray().toHex().data();
    qDebug() << strDis;
    qDebug() << QString::number(static_cast<int>(writeunit.valueCount()), 10);
    for (int i = 0; i < static_cast<int>(writeunit.valueCount()); i++) {
        int j = 4 * i;
        if (j >= strDis.length())
            continue;
        QString st = strDis.mid(j, 4);
        bool ok;
        int hex = st.toInt(&ok, 16);                      //将textedit中读取到的数据转换为16进制发送
        writeunit.setValue(i, static_cast<quint16>(hex)); //设置发送数据
    }

    if (QModbusReply *reply = modbusDevice->sendWriteRequest(writeunit, 1)) {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, [this, reply]() {
                if (reply->error() == QModbusDevice::ProtocolError) {
                    statusBar()->showMessage(tr("write resposne error:%1(Modbus exception:0x%02)")
                                                 .arg(reply->errorString())
                                                 .arg(reply->rawResult().exceptionCode(), -1, 16),
                        5000);
                } else if (reply->error() != QModbusDevice::NoError) {
                    statusBar()->showMessage(tr("write response error:%1(code:0x%2)")
                                                 .arg(reply->errorString())
                                                 .arg(reply->error(), -1, 16),
                        5000);
                }
                reply->deleteLater();
            });
        } else {
            reply->deleteLater();
        }
    } else {
        statusBar()->showMessage(tr("Write error: ") + modbusDevice->errorString(), 5000);
    }
#endif
}

void MainWindow::on_requestButton_clicked() {
#ifdef CHANAL_SERIAL

#elif defined CHANAL_MODBUS
    if (!modbusDevice)
        return;
    statusBar()->clearMessage(); //清除状态栏显示的信息
    Parse_Data write_data;
    write_data.Parse_insert_flag(tr("0x0505"));
    write_data.Parse_insert_deviceName(ui->lineEdit_writeDeviceName->text());
    write_data.Parse_insert_productKey(ui->lineEdit_writeProductKey->text());
    write_data.Parse_insert_deviceSecret(ui->lineEdit_writeDeviceSecret->text());
    write_data.Parse_insert_clientID(ui->lineEdit_writeClientID->text());
    //      QString strDis = write_data.GetArray().toHex().data();

    QModbusRequest writeRequest(QModbusPdu::WriteMultipleRegisters, write_data.GetArray());

    if (QModbusReply *reply = modbusDevice->sendRawRequest(writeRequest, 1)) {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, [this, reply]() {
                if (reply->error() == QModbusDevice::ProtocolError) {
                    statusBar()->showMessage(tr("write resposne error:%1(Modbus exception:0x%02)")
                                                 .arg(reply->errorString())
                                                 .arg(reply->rawResult().exceptionCode(), -1, 16),
                        5000);
                } else if (reply->error() != QModbusDevice::NoError) {
                    statusBar()->showMessage(tr("write response error:%1(code:0x%2)")
                                                 .arg(reply->errorString())
                                                 .arg(reply->error(), -1, 16),
                        5000);
                }
                reply->deleteLater();
            });
        } else {
            reply->deleteLater();
        }
    } else {
        statusBar()->showMessage(tr("Write error: ") + modbusDevice->errorString(), 5000);
    }
#endif
}

void MainWindow::time_up() {
    if (ReceiveBuf.isEmpty())
        return;
    if (JQChecksum::crc16ForModbus(ReceiveBuf) != 0) {
        statusBar()->showMessage(tr("checksum error"));
        return;
    }

    if (ReceiveBuf.at(0) != 1)
        goto exit;

    switch (ReceiveBuf.at(1)) {
        case QModbusPdu::ReadHoldingRegisters: //0x10
        {
            qDebug() << tr("data count:") << QString::number(ReceiveBuf.at(2), 10);
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
            QByteArray array_temp;

            int flag = P_data.info.flag;
            qDebug() << flag;
            ui->lineEdit_readFlag->setText(QString::number(P_data.info.flag, 16));
            ui->lineEdit_readDeviceName->setText(QString::fromLocal8Bit(
                P_data.info.device_name, static_cast<int>(strlen(P_data.info.device_name))));
            ui->lineEdit_readProductKey->setText(QString::fromLocal8Bit(
                P_data.info.product_key, static_cast<int>(strlen(P_data.info.product_key))));
            ui->lineEdit_readDeviceSecret->setText(QString::fromLocal8Bit(
                P_data.info.device_secret, static_cast<int>(strlen(P_data.info.device_secret))));
            ui->lineEdit_readClientID->setText(QString::fromLocal8Bit(
                P_data.info.device_id, static_cast<int>(strlen(P_data.info.device_id))));
            statusBar()->showMessage(tr("read sucess"));
            break;
        }

        case QModbusPdu::WriteMultipleRegisters: {
            statusBar()->showMessage(tr("write sucess"));
            break;
        }
        default:
            statusBar()->showMessage(tr("function code error"));
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
    } else {
        ui->lineEdit_writeDeviceName->setReadOnly(false);
        ui->lineEdit_writeProductKey->setReadOnly(false);
        ui->lineEdit_writeDeviceSecret->setReadOnly(false);
        ui->lineEdit_writeClientID->setReadOnly(false);
    }
}

void MainWindow::key_time_out() {
    qDebug() << currKey;
    ui->lineEdit_writeDeviceName->clear();
}
