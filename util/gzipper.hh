#pragma once

#include <QObject>

QT_BEGIN_NAMESPACE
class QFileInfo;
class QByteArray;
QT_END_NAMESPACE

namespace Freebox {

class Gzipper : public QObject
{
    Q_OBJECT

public:
    Gzipper() {}
    static QByteArray gzip(QByteArray &ba);

private:
    static quint32 CRC32(const QByteArray& data);
};

} // namespace Freebox

