#include "parse_data.h"
#include <QDebug>

Parse_Data::Parse_Data(QWidget *parent)
    : QMainWindow(parent) {
    clearData();
}

void Parse_Data::clearData() {
    memset(&info, 0, sizeof(iotx_device_info_t));
}

void Parse_Data::Parse_Read(QByteArray data) {
    Parse(data);
}

void Parse_Data::Parse_insert_flag(QString flag) {
    bool ok;
    info.flag = flag.toUShort(&ok, 16);
    qDebug() << QString::number(info.flag, 16);
}

void Parse_Data::Parse_insert_clientID(QString clientID) {
    QByteArray idArray = clientID.toLocal8Bit();
    uint len = clientID.length() > DEVICE_ID_LEN
        ? DEVICE_ID_LEN
        : static_cast<uint>(clientID.length());
    strncpy(info.device_id, idArray.data(), len);
    qDebug() << QString::fromLocal8Bit(info.device_id,
        static_cast<int>(strlen(info.device_id)));
}

void Parse_Data::Parse_insert_deviceName(QString name) {
    QByteArray nameArray = name.toLocal8Bit();
    uint len = name.length() > DEVICE_NAME_LEN ? DEVICE_NAME_LEN
                                               : static_cast<uint>(name.length());
    strncpy(info.device_name, nameArray.data(), len);
    qDebug() << QString::fromLocal8Bit(
        info.device_name, static_cast<int>(strlen(info.device_name)));
}

void Parse_Data::Parse_insert_productKey(QString key) {
    QByteArray keyArray = key.toLocal8Bit();
    uint len = key.length() > PRODUCT_KEY_LEN ? PRODUCT_KEY_LEN
                                              : static_cast<uint>(key.length());
    strncpy(info.product_key, keyArray.data(), len);
    qDebug() << QString::fromLocal8Bit(
        info.product_key, static_cast<int>(strlen(info.product_key)));
}

void Parse_Data::Parse_insert_deviceSecret(QString secret) {
    QByteArray secretArray = secret.toLocal8Bit();
    uint len = secret.length() > DEVICE_SECRET_LEN
        ? DEVICE_SECRET_LEN
        : static_cast<uint>(secret.length());

    strncpy(info.device_secret, secretArray.data(), len);
    qDebug() << QString::fromLocal8Bit(
        info.device_secret, static_cast<int>(strlen(info.device_secret)));
}

QByteArray Parse_Data::GetArray() {
    QByteArray buff;
    buff.resize(200 /*static_cast<int>(sizeof(iotx_device_info_t))*/);
    memcpy(buff.data(), &info, sizeof(iotx_device_info_t));
    return buff;
}

void Parse_Data::Parse(QByteArray data) {
    memset(&info, 0, sizeof(iotx_device_info_t));
    memcpy(&info, data.data(), sizeof(iotx_device_info_t));
}
