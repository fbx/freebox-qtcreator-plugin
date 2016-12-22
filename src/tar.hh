#pragma once

#include "freestorepackager.hh"

#include <QObject>

class QFileInfo;
class QByteArray;

namespace Freebox {
namespace Internal {

class Tar : public QObject
{
    Q_OBJECT

public:
    Tar() {}
    QByteArray package(QMap<int, FreeStorePackageFile> packages);

private:
    bool addFile(QByteArray &ba, const FreeStorePackageFile &pf);
    QByteArray entryHeader(const FreeStorePackageFile &pf);
};

} // namespace Internal
} // namespace Freebox

