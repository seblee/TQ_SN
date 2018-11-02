#ifndef JQCHECKSUM_H
#define JQCHECKSUM_H

#include <QByteArray>

namespace JQChecksum {

quint16 crc16ForModbus(const QByteArray &data);

quint16 crc16ForX25(const QByteArray &data);

quint32 crc32(const QByteArray &data);
};

#endif // JQCHECKSUM_H
