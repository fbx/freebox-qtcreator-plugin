#include <QByteArray>
#include <QDataStream>

#include "gzipper.hh"

extern "C" {
# include "crc32.h"
}

using namespace Freebox;

QByteArray Gzipper::gzip(QByteArray &ba)
{
    QByteArray gzBa = qCompress(ba, 9);
    gzBa.remove(0, 6);
    gzBa.remove(gzBa.size() - 4, 4);

    QByteArray header;
    QDataStream ds1(&header, QIODevice::WriteOnly);
    // Prepend a generic 10-byte gzip header (see RFC 1952),
    ds1 << quint16(0x1f8b)
        << quint16(0x0800)
        << quint16(0x0000)
        << quint16(0x0000)
        << quint16(0x000b);

    // Append a four-byte CRC-32 of the uncompressed data
    // Append 4 bytes uncompressed input size modulo 2^32
    QByteArray footer;
    QDataStream ds2(&footer, QIODevice::WriteOnly);
    ds2.setByteOrder(QDataStream::LittleEndian);

    ds2 << CRC32(ba)
        << quint32(ba.size());

    return header + gzBa + footer;
}

quint32 Gzipper::CRC32(const QByteArray& data)
{
    return crc32(~0, (const unsigned char *) data.constData(), data.size()) ^ ~0;
}
