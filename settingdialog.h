#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>
#include <QSerialPort>

namespace Ui {
class settingDialog;
}

class settingDialog : public QDialog {
    Q_OBJECT

  public:
    explicit settingDialog(QWidget *parent = nullptr);
    ~settingDialog();

    struct Settings {
        QSerialPort::Parity parity = QSerialPort::NoParity;
        QSerialPort::BaudRate baud = QSerialPort::Baud19200;
        QSerialPort::DataBits dataBits = QSerialPort::Data8;
        QSerialPort::StopBits stopBits = QSerialPort::OneStop;
        int responseTime = 1000;
        int numberOfRetries = 3;
    };

    Settings settings() const;

    QSerialPort *serial() const;
  signals:
    void connect_button_pushed();
  private slots:
    void on_applyButton_clicked();

    void on_pushButton_refresh_clicked();

  private:
    Settings m_settings;
    Ui::settingDialog *ui;

    QSerialPort *m_serial;
};

#endif // SETTINGDIALOG_H
