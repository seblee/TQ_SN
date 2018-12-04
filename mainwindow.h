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

    void on_connectButton_clicked();

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

  private:
    Ui::MainWindow *ui;

    QTimer *timer;

    QSerialPort *serial;

    QByteArray ReceiveBuf;

    void keyPressEvent(QKeyEvent *event);

    QTimer *K_timer;

    QString currKey;

    settingDialog *m_settingsDialog;

    ExcelManger *excel;
};

#endif // MAINWINDOW_H
