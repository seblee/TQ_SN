#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "excelmanger.h"
#include <QKeyEvent>
#include <QMainWindow>
#include <QModbusClient>
#include <QSerialPortInfo>
#include <QTimer>
#include <QtSerialPort>

namespace Ui {
class MainWindow;
}
class settingDialog;

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

  protected slots:
    void Read_Data();

  private slots:
    void handleError(QSerialPort::SerialPortError error);
    void connectButton_clicked();
    void onStateChanged(int states);
    void on_readButton_clicked();
    void on_writeButton_clicked();
    void time_up();
    void on_checkBox_ScanSwith_stateChanged(int arg1);
    void key_time_out();
    void on_pushButton_clicked();
    void on_actionOptions_triggered();
    void on_actionConnect_triggered();
    void on_actionDisconnect_triggered();
    void write_to_file(QJsonDocument data);

    void checkout_connect();

  private:
    Ui::MainWindow *ui;
    QTimer *timer;
    QTimer *Connect_timer;
    QSerialPort *serial;
    QByteArray ReceiveBuf;
    void keyPressEvent(QKeyEvent *event);
    QTimer *K_timer;
    settingDialog *m_settingsDialog;
    ExcelManger *excel;
    void send_to_bord(QString deviceName, QString productKey, QString deviceSecret);

    int Retries;

    bool checkWrite;
    bool needtowrite;

    QString readFlag;
    QString device_name;
    QString product_key;
    QString device_secret;
    QString device_id;
};

#endif // MAINWINDOW_H
