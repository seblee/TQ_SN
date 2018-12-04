#ifndef AUTHREGIST_H
#define AUTHREGIST_H

#include <QMainWindow>

#define REGISTER_PRODUCT_KEY "a1JOOi3mNEf"
#define REGISTER_PRODUCT_SECRET "BQjNWOG8EJWa4nFu"

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
    explicit AuthRegist(QString SID);

  private:
    QString SID;
    QByteArray request;

    QByteArray utils_hmac_md5(const char *msg, int msg_len, const char *key, int key_len);
    void utils_md5_init(iot_md5_context *ctx);
    void utils_md5_starts(iot_md5_context *ctx);
    void utils_md5_update(iot_md5_context *ctx, const unsigned char *input, size_t ilen);
    void utils_md5_finish(iot_md5_context *ctx, unsigned char output[16]);
    void utils_md5_process(iot_md5_context *ctx, const unsigned char data[64]);
    int8_t utils_hb2hex(uint8_t hb);
  signals:

  public slots:
};

#endif // AUTHREGIST_H
