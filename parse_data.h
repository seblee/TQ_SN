#ifndef PARSE_DATA_H
#define PARSE_DATA_H

#include <QMainWindow>

#define PRODUCT_KEY_LEN (20)
#define DEVICE_NAME_LEN (32)
#define DEVICE_ID_LEN (64)
#define DEVICE_SECRET_LEN (64)

typedef struct {
    quint16 flag;
    char device_name[DEVICE_NAME_LEN + 1];
    char product_key[PRODUCT_KEY_LEN + 1];
    char device_secret[DEVICE_SECRET_LEN + 1];
    char device_id[DEVICE_ID_LEN + 1];
} iotx_device_info_t, *iotx_device_info_pt;

class Parse_Data : public QMainWindow {
    Q_OBJECT
  public:
    explicit Parse_Data(QWidget *parent = nullptr);

    void Parse_Read(QByteArray data);

    void Parse_insert_flag(QString flag);

    void Parse_insert_clientID(QString clientID);

    void Parse_insert_deviceName(QString name);

    void Parse_insert_productKey(QString key);

    void Parse_insert_deviceSecret(QString secret);

    iotx_device_info_t info;

    QByteArray GetArray();
  signals:

  public slots:

  private:
    void
        Parse(QByteArray data);

    void clearData();
};

#endif // PARSE_DATA_H
