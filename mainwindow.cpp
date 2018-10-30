#include "mainwindow.h"
#include "mainwindow.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    serial(nullptr)
{
    serial = new QSerialPort;
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    qDebug() << "SerialPortError " << QString::number(error, 10);
    if (!serial)
        return;
    if (error == QSerialPort::ResourceError)
    {
        QMessageBox::critical(this, tr("Critical Error"), serial->errorString());
        serial->clear();
        serial->close();
        ui->connectButton->setText(tr("Connect"));
        ui->CheckBox_SerialStatus->setCheckState(Qt::Unchecked);
    }
    else if (error == QSerialPort::PermissionError)
    {
        QMessageBox::critical(this, tr("Critical Error"), tr("\r\nThe serial has been already opened\r\nPlease Check"));
        serial->clear();
        serial->close();
        ui->connectButton->setText(tr("Connect"));
        ui->CheckBox_SerialStatus->setCheckState(Qt::Unchecked);
    }
}



void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        this->close();
    }
}
void MainWindow::on_pushButton_2_clicked()
{
    ui->Serial_item->clear();
    foreach (const QSerialPortInfo &com_info, QSerialPortInfo::availablePorts())
    {
        qDebug() <<  com_info.portName() << com_info.description();
        ui->Serial_item->addItem(com_info.portName() + QWidget::tr("   ") + com_info.description());
    }
}

void MainWindow::Read_Data()
{
    static QByteArray buf;

    buf += serial->readAll();
    if (!buf.isEmpty())
    {

        buf.clear();
    }
}

void MainWindow::on_connectButton_clicked()
{
    QString str = ui->Serial_item->currentText();
    if (!serial)
        return;
    statusBar()->clearMessage();
    if(serial->openMode() != QIODevice::ReadWrite)
    {
        if( str.length() == 0){
            ui->statusBar->showMessage("Serial is empty",3000);
            return;
        }
        connect(serial, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error), this, &MainWindow::handleError); //连接槽，串口出现问题连接到错误处理函数
        serial->setPortName(str.section("  ",0,0));
        if (serial->open(QIODevice::ReadWrite) == true)
        {
            serial->setBaudRate(19200);
            serial->setDataBits(QSerialPort::Data8);
            serial->setParity(QSerialPort::NoParity);
            serial->setStopBits(QSerialPort::OneStop);
            QObject::connect(serial, &QSerialPort::readyRead, this, &MainWindow::Read_Data);

            qDebug() << "open serial" << serial->portName() << "done";
            ui->connectButton->setText(tr("Disconnect"));
            ui->CheckBox_SerialStatus->setCheckState(Qt::Checked);
        }
       statusBar()->showMessage(str.section("  ",0,0) + tr(" Connected"));
    }
    else
    {
        serial->clear();
        serial->close();
        ui->connectButton->setText(tr("Connect"));
        ui->CheckBox_SerialStatus->setCheckState(Qt::Unchecked);
        statusBar()->showMessage(tr("Serial ") + tr("Disconnected"));
    }
}
