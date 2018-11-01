#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QKeyEvent>
#include <QMainWindow>
#include <QModbusClient>
#include <QSerialPortInfo>
#include <QTimer>
#include <QtSerialPort>

namespace Ui {
class MainWindow;
}

#define CHANAL_SERIAL
//#define CHANAL_MODBUS

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

  protected slots:
#ifdef CHANAL_SERIAL
    void Read_Data();
#elif defined CHANAL_MODBUS
    void readReady();
#endif
  private slots:
    void handleError(QSerialPort::SerialPortError error);

    void on_pushButton_2_clicked();

    void on_connectButton_clicked();

    void onStateChanged(int states);

    void on_readButton_clicked();

    void on_writeButton_clicked();

    void on_requestButton_clicked();

    void time_up();

  private:
    Ui::MainWindow *ui;
    QTimer *timer;
#ifdef CHANAL_SERIAL
    QSerialPort *serial;
    QByteArray buf;
#elif defined CHANAL_MODBUS
    QModbusClient *modbusDevice;
#endif
    void
        keyPressEvent(QKeyEvent *event);
};

#endif // MAINWINDOW_H
