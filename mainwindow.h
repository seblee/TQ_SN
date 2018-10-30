#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort>
#include <QSerialPortInfo>
#include <QKeyEvent>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
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

private:
    Ui::MainWindow *ui;

    QSerialPort *serial;

    void keyPressEvent(QKeyEvent *event);
};

#endif // MAINWINDOW_H
