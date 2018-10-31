#include "mainwindow.h"
#include "parse_data.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QModbusDataUnit>
#include <QModbusReply>
#include <QModbusRtuSerialMaster>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , serial(nullptr)
    , modbusDevice(nullptr) {
    serial = new QSerialPort;
    ui->setupUi(this);

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
}

MainWindow::~MainWindow() {
    if (modbusDevice)
        if (modbusDevice->state() == QModbusDevice::ConnectedState)
            modbusDevice->disconnectDevice();
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
        if (modbusDevice)
            if (modbusDevice->state() == QModbusDevice::ConnectedState)
                modbusDevice->disconnectDevice();
        this->close();
    }
}
void MainWindow::on_pushButton_2_clicked() {
    ui->Serial_item->clear();
    foreach (const QSerialPortInfo &com_info, QSerialPortInfo::availablePorts()) {
        qDebug() << com_info.portName() << com_info.description();
        ui->Serial_item->addItem(com_info.portName() + QWidget::tr("   ") +
            com_info.description());
    }
}

void MainWindow::Read_Data() {
    static QByteArray buf;

    buf += serial->readAll();
    if (!buf.isEmpty()) {
        buf.clear();
    }
}

void MainWindow::on_connectButton_clicked() {
    //    QString str = ui->Serial_item->currentText();
    //    if (!serial)
    //        return;
    //    statusBar()->clearMessage();
    //    if(serial->openMode() != QIODevice::ReadWrite)
    //    {
    //        if( str.length() == 0){
    //            ui->statusBar->showMessage("Serial is empty",3000);
    //            return;
    //        }
    //        connect(serial, static_cast<void
    //        (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
    //        this, &MainWindow::handleError);
    //        //连接槽，串口出现问题连接到错误处理函数
    //        serial->setPortName(str.section("  ",0,0));
    //        if (serial->open(QIODevice::ReadWrite) == true)
    //        {
    //            serial->setBaudRate(QSerialPort::Baud19200);
    //            serial->setDataBits(QSerialPort::Data8);
    //            serial->setParity(QSerialPort::NoParity);
    //            serial->setStopBits(QSerialPort::OneStop);
    //            QObject::connect(serial, &QSerialPort::readyRead, this,
    //            &MainWindow::Read_Data);

    //            qDebug() << "open serial" << serial->portName() << "done";
    //            ui->connectButton->setText(tr("Disconnect"));
    //            ui->CheckBox_SerialStatus->setCheckState(Qt::Checked);
    //        }
    //       statusBar()->showMessage(str.section("  ",0,0) + tr(" Connected"));
    //    }
    //    else
    //    {
    //        serial->clearError();
    //        serial->clear();
    //        serial->close();
    //        ui->connectButton->setText(tr("Connect"));
    //        ui->CheckBox_SerialStatus->setCheckState(Qt::Unchecked);
    //        statusBar()->showMessage(tr("Serial ") + tr("Disconnected"));
    //    }

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
        modbusDevice->setNumberOfRetries(3);
        if (!modbusDevice->connectDevice()) {
            statusBar()->showMessage(tr("Connect failed: ") +
                modbusDevice->errorString());
        } else {
            statusBar()->showMessage(tr("Connect sucess"));
        }
    } else {
        modbusDevice->disconnectDevice();
    }
}

void MainWindow::on_readButton_clicked() {
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
}

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

void MainWindow::on_writeButton_clicked() {
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

    for (int i = 0; i < static_cast<int>(writeunit.valueCount()); i++) {
        int j = 2 * i;
        if (j >= strDis.length())
            continue;
        QString st = strDis.mid(j, 2);
        bool ok;
        int hex = st.toInt(&ok, 16); //将textedit中读取到的数据转换为16进制发送
        qDebug() << hex;
        writeunit.setValue(i, static_cast<quint16>(hex)); //设置发送数据
    }
    QByteArray data;
    for (uint i = 0; i < writeunit.valueCount(); i++) {
        data.insert(static_cast<int>(i * 2),
            static_cast<char>(writeunit.value(static_cast<int>(i)) >> 8));
        data.insert(static_cast<int>(i * 2 + 1),
            static_cast<char>(writeunit.value(static_cast<int>(i)) & 0x00ff));
    }

    //    if (auto *reply = modbusDevice->sendWriteRequest(writeUnit, 1)) { // 1是server address   sendWriteRequest是向服务器写数据
    //        if (!reply->isFinished()) {                                   //reply Returns true when the reply has finished or was aborted.
    //            connect(reply, &QModbusReply::finished, this, [this, reply]() {
    //                if (reply->error() == QModbusDevice::ProtocolError) {
    //                    statusBar()->showMessage(tr("Write response error: %1 (Mobus exception: 0x%2)")
    //                                                 .arg(reply->errorString())
    //                                                 .arg(reply->rawResult().exceptionCode(), -1, 16),
    //                        5000);
    //                } else if (reply->error() != QModbusDevice::NoError) {
    //                    statusBar()->showMessage(tr("Write response error: %1 (code: 0x%2)").arg(reply->errorString()).arg(reply->error(), -1, 16), 5000);
    //                }
    //                reply->deleteLater();
    //            });
    //        } else {
    //            // broadcast replies return immediately
    //            reply->deleteLater();
    //        }
    //    } else {
    //        statusBar()->showMessage(tr("Write error: ") + modbusDevice->errorString(), 5000);
    //    }
}
