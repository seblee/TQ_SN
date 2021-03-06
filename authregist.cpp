#include "authregist.h"
#include <QDebug>
#include <QJsonObject>
#include <QMessageBox>
#include <QTime>

static const unsigned char iot_md5_padding[64] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

AuthRegist::AuthRegist(QWidget *parent)
    : QMainWindow(parent)
    , m_AccessManager(nullptr) {
}

AuthRegist::AuthRegist(QString deviceName, QString productModle) {
    int srandom;
    QByteArray md5_scr;
    const char *produce_key;
    const char *produce_secret;
    md5_scr.append("deviceName");
    md5_scr.append(deviceName);
    md5_scr.append("productKey");
    if (productModle.lastIndexOf(tr("L28 ")) == 0) {
        md5_scr.append(L280_PRODUCT_KEY);
        produce_key = L280_PRODUCT_KEY;
        produce_secret = L280_PRODUCT_SECRET;
    } else if (productModle.lastIndexOf(tr("L50R")) == 0) {
        md5_scr.append(L50R_PRODUCT_KEY);
        produce_key = L50R_PRODUCT_KEY;
        produce_secret = L50R_PRODUCT_SECRET;
    } else if (productModle.lastIndexOf(tr("L60W")) == 0) {
        md5_scr.append(L60W_PRODUCT_KEY);
        produce_key = L60W_PRODUCT_KEY;
        produce_secret = L60W_PRODUCT_SECRET;
    } else {
        md5_scr.append(L60W_PRODUCT_KEY);
        produce_key = L60W_PRODUCT_KEY;
        produce_secret = L60W_PRODUCT_SECRET;
    }
    md5_scr.append("random");
    qsrand(static_cast<uint>(QTime(0, 0, 0).secsTo(QTime::currentTime())));
    srandom = qrand();
    md5_scr.append(QString::number(srandom, 10));

    QByteArray guider_sign;
    guider_sign = utils_hmac_md5(md5_scr.data(), md5_scr.length(),
        produce_secret,
        strlen(produce_secret));

    body.append(
        QString("productKey=%1&deviceName=%2&random=%3&sign=%4&signMethod=HmacMD5")
            .arg(produce_key)
            .arg(deviceName)
            .arg(QString::number(srandom, 10))
            .arg(QString::fromLatin1(guider_sign)));
    qDebug() << body;
}

void AuthRegist::handleTimeOut() {
    timer.stop();
    qDebug() << "time out";
    QJsonObject jsonObj;
    jsonObj.insert("error", "time out");
    QJsonDocument json;
    json.setObject(jsonObj);
    emit register_back(json);
    m_reply->abort();
    m_reply->deleteLater();
}

QByteArray AuthRegist::utils_hmac_md5(const char *msg, int msg_len, const char *key, unsigned int key_len) {
    QByteArray digest;
    if ((nullptr == msg) || (nullptr == key)) {
        // log_err("parameter is Null,failed!");
        return digest;
    }

    if (key_len > KEY_IOPAD_SIZE) {
        // log_err("key_len > size(%d) of array", KEY_IOPAD_SIZE);
        return digest;
    }

    iot_md5_context context;

    unsigned char k_ipad[KEY_IOPAD_SIZE]; /* inner padding - key XORd with ipad  */
    unsigned char k_opad[KEY_IOPAD_SIZE]; /* outer padding - key XORd with opad */
    unsigned char out[MD5_DIGEST_SIZE];
    int i;

    /* start out by storing key in pads */
    memset(k_ipad, 0, sizeof(k_ipad));
    memset(k_opad, 0, sizeof(k_opad));
    memcpy(k_ipad, key, static_cast<size_t>(key_len));
    memcpy(k_opad, key, static_cast<size_t>(key_len));

    /* XOR key with ipad and opad values */
    for (i = 0; i < KEY_IOPAD_SIZE; i++) {
        k_ipad[i] ^= 0x36;
        k_opad[i] ^= 0x5c;
    }

    /* perform inner MD5 */
    utils_md5_init(&context);                                                       /* init context for 1st pass */
    utils_md5_starts(&context);                                                     /* setup context for 1st pass */
    utils_md5_update(&context, k_ipad, KEY_IOPAD_SIZE);                             /* start with inner pad */
    utils_md5_update(&context, (unsigned char *)msg, static_cast<size_t>(msg_len)); /* then text of datagram */
    utils_md5_finish(&context, out);                                                /* finish up 1st pass */

    /* perform outer MD5 */
    utils_md5_init(&context);                           /* init context for 2nd pass */
    utils_md5_starts(&context);                         /* setup context for 2nd pass */
    utils_md5_update(&context, k_opad, KEY_IOPAD_SIZE); /* start with outer pad */
    utils_md5_update(&context, out, MD5_DIGEST_SIZE);   /* then results of 1st hash */
    utils_md5_finish(&context, out);                    /* finish up 2nd pass */

    for (i = 0; i < MD5_DIGEST_SIZE; ++i) {
        digest.insert(i * 2, utils_hb2hex(out[i] >> 4));
        digest.insert(i * 2 + 1, utils_hb2hex(out[i]));
    }
    return digest;
}

void AuthRegist::utils_md5_init(iot_md5_context *ctx) {
    memset(ctx, 0, sizeof(iot_md5_context));
}

void AuthRegist::utils_md5_starts(iot_md5_context *ctx) {
    ctx->total[0] = 0;
    ctx->total[1] = 0;

    ctx->state[0] = 0x67452301;
    ctx->state[1] = 0xEFCDAB89;
    ctx->state[2] = 0x98BADCFE;
    ctx->state[3] = 0x10325476;
}

void AuthRegist::utils_md5_update(iot_md5_context *ctx, const unsigned char *input, size_t ilen) {
    size_t fill;
    uint32_t left;

    if (ilen == 0) {
        return;
    }

    left = ctx->total[0] & 0x3F;
    fill = 64 - left;

    ctx->total[0] += static_cast<uint32_t>(ilen);
    ctx->total[0] &= 0xFFFFFFFF;

    if (ctx->total[0] < static_cast<uint32_t>(ilen)) {
        ctx->total[1]++;
    }

    if (left && ilen >= fill) {
        memcpy(static_cast<void *>(ctx->buffer + left), input, fill);
        utils_md5_process(ctx, ctx->buffer);
        input += fill;
        ilen -= fill;
        left = 0;
    }

    while (ilen >= 64) {
        utils_md5_process(ctx, input);
        input += 64;
        ilen -= 64;
    }

    if (ilen > 0) {
        memcpy(static_cast<void *>(ctx->buffer + left), input, ilen);
    }
}

void AuthRegist::utils_md5_finish(iot_md5_context *ctx, unsigned char output[]) {

    uint32_t last, padn;
    uint32_t high, low;
    unsigned char msglen[8];

    high = (ctx->total[0] >> 29) | (ctx->total[1] << 3);
    low = (ctx->total[0] << 3);

    IOT_MD5_PUT_UINT32_LE(low, msglen, 0);
    IOT_MD5_PUT_UINT32_LE(high, msglen, 4);

    last = ctx->total[0] & 0x3F;
    padn = (last < 56) ? (56 - last) : (120 - last);

    utils_md5_update(ctx, iot_md5_padding, padn);
    utils_md5_update(ctx, msglen, 8);

    IOT_MD5_PUT_UINT32_LE(ctx->state[0], output, 0);
    IOT_MD5_PUT_UINT32_LE(ctx->state[1], output, 4);
    IOT_MD5_PUT_UINT32_LE(ctx->state[2], output, 8);
    IOT_MD5_PUT_UINT32_LE(ctx->state[3], output, 12);
}

void AuthRegist::utils_md5_process(iot_md5_context *ctx, const unsigned char data[]) {
    uint32_t X[16], A, B, C, D;

    IOT_MD5_GET_UINT32_LE(X[0], data, 0);
    IOT_MD5_GET_UINT32_LE(X[1], data, 4);
    IOT_MD5_GET_UINT32_LE(X[2], data, 8);
    IOT_MD5_GET_UINT32_LE(X[3], data, 12);
    IOT_MD5_GET_UINT32_LE(X[4], data, 16);
    IOT_MD5_GET_UINT32_LE(X[5], data, 20);
    IOT_MD5_GET_UINT32_LE(X[6], data, 24);
    IOT_MD5_GET_UINT32_LE(X[7], data, 28);
    IOT_MD5_GET_UINT32_LE(X[8], data, 32);
    IOT_MD5_GET_UINT32_LE(X[9], data, 36);
    IOT_MD5_GET_UINT32_LE(X[10], data, 40);
    IOT_MD5_GET_UINT32_LE(X[11], data, 44);
    IOT_MD5_GET_UINT32_LE(X[12], data, 48);
    IOT_MD5_GET_UINT32_LE(X[13], data, 52);
    IOT_MD5_GET_UINT32_LE(X[14], data, 56);
    IOT_MD5_GET_UINT32_LE(X[15], data, 60);

#define S(x, n) ((x << n) | ((x & 0xFFFFFFFF) >> (32 - n)))

#define P(a, b, c, d, k, s, t)      \
    {                               \
        a += F(b, c, d) + X[k] + t; \
        a = S(a, s) + b;            \
    }

    A = ctx->state[0];
    B = ctx->state[1];
    C = ctx->state[2];
    D = ctx->state[3];

#define F(x, y, z) (z ^ (x & (y ^ z)))

    P(A, B, C, D, 0, 7, 0xD76AA478);
    P(D, A, B, C, 1, 12, 0xE8C7B756);
    P(C, D, A, B, 2, 17, 0x242070DB);
    P(B, C, D, A, 3, 22, 0xC1BDCEEE);
    P(A, B, C, D, 4, 7, 0xF57C0FAF);
    P(D, A, B, C, 5, 12, 0x4787C62A);
    P(C, D, A, B, 6, 17, 0xA8304613);
    P(B, C, D, A, 7, 22, 0xFD469501);
    P(A, B, C, D, 8, 7, 0x698098D8);
    P(D, A, B, C, 9, 12, 0x8B44F7AF);
    P(C, D, A, B, 10, 17, 0xFFFF5BB1);
    P(B, C, D, A, 11, 22, 0x895CD7BE);
    P(A, B, C, D, 12, 7, 0x6B901122);
    P(D, A, B, C, 13, 12, 0xFD987193);
    P(C, D, A, B, 14, 17, 0xA679438E);
    P(B, C, D, A, 15, 22, 0x49B40821);

#undef F

#define F(x, y, z) (y ^ (z & (x ^ y)))

    P(A, B, C, D, 1, 5, 0xF61E2562);
    P(D, A, B, C, 6, 9, 0xC040B340);
    P(C, D, A, B, 11, 14, 0x265E5A51);
    P(B, C, D, A, 0, 20, 0xE9B6C7AA);
    P(A, B, C, D, 5, 5, 0xD62F105D);
    P(D, A, B, C, 10, 9, 0x02441453);
    P(C, D, A, B, 15, 14, 0xD8A1E681);
    P(B, C, D, A, 4, 20, 0xE7D3FBC8);
    P(A, B, C, D, 9, 5, 0x21E1CDE6);
    P(D, A, B, C, 14, 9, 0xC33707D6);
    P(C, D, A, B, 3, 14, 0xF4D50D87);
    P(B, C, D, A, 8, 20, 0x455A14ED);
    P(A, B, C, D, 13, 5, 0xA9E3E905);
    P(D, A, B, C, 2, 9, 0xFCEFA3F8);
    P(C, D, A, B, 7, 14, 0x676F02D9);
    P(B, C, D, A, 12, 20, 0x8D2A4C8A);

#undef F

#define F(x, y, z) (x ^ y ^ z)

    P(A, B, C, D, 5, 4, 0xFFFA3942);
    P(D, A, B, C, 8, 11, 0x8771F681);
    P(C, D, A, B, 11, 16, 0x6D9D6122);
    P(B, C, D, A, 14, 23, 0xFDE5380C);
    P(A, B, C, D, 1, 4, 0xA4BEEA44);
    P(D, A, B, C, 4, 11, 0x4BDECFA9);
    P(C, D, A, B, 7, 16, 0xF6BB4B60);
    P(B, C, D, A, 10, 23, 0xBEBFBC70);
    P(A, B, C, D, 13, 4, 0x289B7EC6);
    P(D, A, B, C, 0, 11, 0xEAA127FA);
    P(C, D, A, B, 3, 16, 0xD4EF3085);
    P(B, C, D, A, 6, 23, 0x04881D05);
    P(A, B, C, D, 9, 4, 0xD9D4D039);
    P(D, A, B, C, 12, 11, 0xE6DB99E5);
    P(C, D, A, B, 15, 16, 0x1FA27CF8);
    P(B, C, D, A, 2, 23, 0xC4AC5665);

#undef F

#define F(x, y, z) (y ^ (x | ~z))

    P(A, B, C, D, 0, 6, 0xF4292244);
    P(D, A, B, C, 7, 10, 0x432AFF97);
    P(C, D, A, B, 14, 15, 0xAB9423A7);
    P(B, C, D, A, 5, 21, 0xFC93A039);
    P(A, B, C, D, 12, 6, 0x655B59C3);
    P(D, A, B, C, 3, 10, 0x8F0CCC92);
    P(C, D, A, B, 10, 15, 0xFFEFF47D);
    P(B, C, D, A, 1, 21, 0x85845DD1);
    P(A, B, C, D, 8, 6, 0x6FA87E4F);
    P(D, A, B, C, 15, 10, 0xFE2CE6E0);
    P(C, D, A, B, 6, 15, 0xA3014314);
    P(B, C, D, A, 13, 21, 0x4E0811A1);
    P(A, B, C, D, 4, 6, 0xF7537E82);
    P(D, A, B, C, 11, 10, 0xBD3AF235);
    P(C, D, A, B, 2, 15, 0x2AD7D2BB);
    P(B, C, D, A, 9, 21, 0xEB86D391);

#undef F

    ctx->state[0] += A;
    ctx->state[1] += B;
    ctx->state[2] += C;
    ctx->state[3] += D;
}

int8_t AuthRegist::utils_hb2hex(uint8_t hb) {
    hb = hb & 0xF;
    return static_cast<int8_t>(hb < 10 ? '0' + hb : hb - 10 + 'a');
}

void AuthRegist::ReplyReadFunc(QNetworkReply *reply) {
    if (timer.isActive())
        timer.stop();
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray response = reply->readAll();
        qDebug() << response;
        QJsonParseError jsonError;
        QJsonDocument json = QJsonDocument::fromJson(response, &jsonError);
        emit register_back(json);
    } else {
        QJsonObject jsonObj;
        jsonObj.insert("error", reply->errorString());
        QJsonDocument json;
        json.setObject(jsonObj);
        emit register_back(json);
        QMessageBox::critical(
            this, tr("reply err"),
            tr("reply err:") + reply->errorString());

        qDebug()
            << tr("reply err:") + reply->errorString();
    }
    reply->deleteLater();
}

void AuthRegist::start_request() {
    if (timer.isActive())
        return;
    if (body.length() == 0)
        return;
    QUrl url("https://iot-auth.cn-shanghai.aliyuncs.com/auth/register/device");
    m_AccessManager = new QNetworkAccessManager(this);

    QSslConfiguration config;
    config.setPeerVerifyDepth(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::TlsV1_2);
    m_Request.setSslConfiguration(config);
    m_Request.setUrl(url);

    m_Request.setRawHeader("Host", url.host().toLocal8Bit());
    m_Request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    m_Request.setRawHeader("Content-Length", tr("%1").arg(body.length()).toLocal8Bit());

    m_reply = m_AccessManager->post(m_Request, body);
    connect(m_AccessManager, &QNetworkAccessManager::finished, this, &AuthRegist::ReplyReadFunc);

    disconnect(&timer, &QTimer::timeout, nullptr, nullptr);
    connect(&timer, &QTimer::timeout, this, &AuthRegist::handleTimeOut);

    if (timer.isActive())
        timer.stop();
    timer.start(5000);
}
