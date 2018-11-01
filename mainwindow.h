#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QKeyEvent>
#include <QMainWindow>
#include <QModbusClient>
#include <QSerialPortInfo>
#include <QtSerialPort>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

  protected slots:
    void Read_Data();

  private slots:
    void handleError(QSerialPort::SerialPortError error);

    void on_pushButton_2_clicked();

    void on_connectButton_clicked();

    void onStateChanged(int states);

    void on_readButton_clicked();

    void readReady();

    void on_writeButton_clicked();

    void on_requestButton_clicked();

  private:
    Ui::MainWindow *ui;

    QSerialPort *serial;

    QModbusClient *modbusDevice;

    void keyPressEvent(QKeyEvent *event);
};

#endif // MAINWINDOW_H
