#ifndef AUTHREGIST_H
#define AUTHREGIST_H

#include <QJsonDocument>
#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>

//#define REGISTER_PRODUCT_KEY "rl0bGtKFCYA"
//#define REGISTER_PRODUCT_SECRET "GSR7xdLGIN3takCN"

//#define L60W_PRODUCT_KEY REGISTER_PRODUCT_KEY
//#define L60W_PRODUCT_SECRET REGISTER_PRODUCT_SECRET

#define L60W_PRODUCT_KEY "a1JOOi3mNEf"
#define L60W_PRODUCT_SECRET "BQjNWOG8EJWa4nFu"

#define L280_PRODUCT_KEY L60W_PRODUCT_KEY
#define L280_PRODUCT_SECRET L60W_PRODUCT_SECRET

#define L50R_PRODUCT_KEY L60W_PRODUCT_KEY
#define L50R_PRODUCT_SECRET L60W_PRODUCT_SECRET


#define Modbus_READ_ADD 399
#define Modbus_WRITE_ADD 399

#define KEY_IOPAD_SIZE 64
#define MD5_DIGEST_SIZE 16
#define SHA1_DIGEST_SIZE 20

typedef struct
{
    uint32_t total[2];        /*!< number of bytes processed  */
    uint32_t state[4];        /*!< intermediate digest state  */
    unsigned char buffer[64]; /*!< data block being processed */
} iot_md5_context;

#ifndef IOT_MD5_GET_UINT32_LE
#define IOT_MD5_GET_UINT32_LE(n, b, i)                                                                                                                                                    \
    {                                                                                                                                                                                     \
        (n) = (static_cast<uint32_t>((b)[(i)])) | (static_cast<uint32_t>((b)[(i) + 1]) << 8) | (static_cast<uint32_t>((b)[(i) + 2]) << 16) | (static_cast<uint32_t>((b)[(i) + 3]) << 24); \
    }
#endif

#ifndef IOT_MD5_PUT_UINT32_LE
#define IOT_MD5_PUT_UINT32_LE(n, b, i)                                 \
    {                                                                  \
        (b)[(i)] = static_cast<unsigned char>(((n)) & 0xFF);           \
        (b)[(i) + 1] = static_cast<unsigned char>(((n) >> 8) & 0xFF);  \
        (b)[(i) + 2] = static_cast<unsigned char>(((n) >> 16) & 0xFF); \
        (b)[(i) + 3] = static_cast<unsigned char>(((n) >> 24) & 0xFF); \
    }
#endif

class AuthRegist : public QMainWindow {
    Q_OBJECT
  public:
    explicit AuthRegist(QWidget *parent = nullptr);
    explicit AuthRegist(QString deviceName, QString productModle);

  signals:
    void register_back(QJsonDocument &SID);

  private slots:
    void handleTimeOut();

  public slots:

  private:
    QString SID;
    QByteArray body;

    QByteArray utils_hmac_md5(const char *msg, int msg_len, const char *key, unsigned int key_len);
    void utils_md5_init(iot_md5_context *ctx);
    void utils_md5_starts(iot_md5_context *ctx);
    void utils_md5_update(iot_md5_context *ctx, const unsigned char *input, size_t ilen);
    void utils_md5_finish(iot_md5_context *ctx, unsigned char output[16]);
    void utils_md5_process(iot_md5_context *ctx, const unsigned char data[64]);
    int8_t utils_hb2hex(uint8_t hb);

    QNetworkAccessManager *m_AccessManager;
    QNetworkReply *m_reply;
    QNetworkRequest m_Request;

    void ReplyReadFunc(QNetworkReply *reply);

    QTimer timer;

  public:
    void start_request();
};

#endif // AUTHREGIST_H
