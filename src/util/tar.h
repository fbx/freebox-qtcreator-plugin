#pragma once

#include "../freestorepackager.h"

#include <QObject>

class QFileInfo;
class QByteArray;

namespace Freebox ::Internal {


class Tar : public QObject
{
    Q_OBJECT

public:
    Tar() {}
    QByteArray package(const QMap<QString, FreeStorePackageFile> &packages);

private:
    bool addFile(QByteArray &ba, const FreeStorePackageFile &pf);
    QByteArray entryHeader(const FreeStorePackageFile &pf);
};

} // namespace Freebox::Internal
