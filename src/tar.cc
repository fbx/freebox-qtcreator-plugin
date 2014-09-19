#include <QFileInfo>
#include <QByteArray>

#include <cstring>

#include "tar.hh"
#include "util/gzipper.hh"
#include "freestorepackager.hh"

using namespace Freebox;
using namespace Internal;

// borrowed from .../qt-creator/src/plugins/remotelinux/tarpackagecreationstep.cpp
const int tarBlockSize = 512;

struct TarFileHeader {
    char fileName[100];
    char fileMode[8];
    char uid[8];
    char gid[8];
    char length[12];
    char mtime[12];
    char chksum[8];
    char typeflag;
    char linkname[100];
    char magic[6];
    char version[2];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
    char fileNamePrefix[155];
    char padding[12];
};
// end of borrowed from .../qt-creator/src/plugins/remotelinux/tarpackagecreationstep.cpp

QByteArray Tar::package(QMap<int, FreeStorePackageFile> packages)
{
    QByteArray ba;

    foreach (const FreeStorePackageFile &pf, packages.values())
        if (!addFile(ba, pf))
            continue; // FIXME: fails silently on missing files

    return ba + QByteArray(tarBlockSize, 0);
}

bool Tar::addFile(QByteArray &ba, const FreeStorePackageFile &pf)
{
    ba += entryHeader(pf);

    if (pf.fileInfo.isDir())
        return true;

    ba += pf.content;

    const int blockModulo = pf.content.size() % tarBlockSize;
    if (blockModulo != 0)
        ba += QByteArray(tarBlockSize - blockModulo, 0);

    return true;
}

QByteArray Tar::entryHeader(const FreeStorePackageFile &pf)
{
    QString filePath = pf.fileNameRelative;
    QFileInfo fileInfo = pf.fileInfo;

    // borrowed from .../qt-creator/src/plugins/remotelinux/tarpackagecreationstep.cpp
    TarFileHeader header;
    std::memset(&header, '\0', sizeof header);

    // const int maxFilePathLength = sizeof header.fileNamePrefix + sizeof header.fileName;
    // if (filePathBa.count() > maxFilePathLength) {
    //     raiseError(tr("Cannot add file \"%1\" to tar-archive: path too long.")
    //         .arg(QDir::toNativeSeparators(remoteFilePath)));
    //     return false;
    // }

    const QByteArray filePathBa = filePath.toUtf8();

    const int fileNameBytesToWrite = qMin<int>(filePathBa.length(), sizeof header.fileName);
    const int fileNameOffset = filePathBa.length() - fileNameBytesToWrite;
    std::memcpy(&header.fileName, filePathBa.data() + fileNameOffset, fileNameBytesToWrite);
    if (fileNameOffset > 0)
        std::memcpy(&header.fileNamePrefix, filePathBa.data(), fileNameOffset);
    int permissions = (0400 * fileInfo.permission(QFile::ReadOwner))
        | (0200 * fileInfo.permission(QFile::WriteOwner))
        | (0100 * fileInfo.permission(QFile::ExeOwner))
        | (040 * fileInfo.permission(QFile::ReadGroup))
        | (020 * fileInfo.permission(QFile::WriteGroup))
        | (010 * fileInfo.permission(QFile::ExeGroup))
        | (04 * fileInfo.permission(QFile::ReadOther))
        | (02 * fileInfo.permission(QFile::WriteOther))
        | (01 * fileInfo.permission(QFile::ExeOther));
    const QByteArray permissionString = QString::fromLatin1("%1").arg(permissions,
        sizeof header.fileMode - 1, 8, QLatin1Char('0')).toLatin1();
    std::memcpy(&header.fileMode, permissionString.data(), permissionString.length());
    const QByteArray uidString = QString::fromLatin1("%1").arg(fileInfo.ownerId(),
        sizeof header.uid - 1, 8, QLatin1Char('0')).toLatin1();
    std::memcpy(&header.uid, uidString.data(), uidString.length());
    const QByteArray gidString = QString::fromLatin1("%1").arg(fileInfo.groupId(),
        sizeof header.gid - 1, 8, QLatin1Char('0')).toLatin1();
    std::memcpy(&header.gid, gidString.data(), gidString.length());
    const QByteArray sizeString = QString::fromLatin1("%1").arg((fileInfo.isDir() ? 0 : fileInfo.size()),
        sizeof header.length - 1, 8, QLatin1Char('0')).toLatin1();
    std::memcpy(&header.length, sizeString.data(), sizeString.length());
    const QByteArray mtimeString = QString::fromLatin1("%1").arg(fileInfo.lastModified().toTime_t(),
        sizeof header.mtime - 1, 8, QLatin1Char('0')).toLatin1();
    std::memcpy(&header.mtime, mtimeString.data(), mtimeString.length());
    if (fileInfo.isDir())
        header.typeflag = '5';
    else
        header.typeflag = '0';
    std::memcpy(&header.magic, "ustar", sizeof "ustar");
    std::memcpy(&header.version, "00", 2);
    const QByteArray &owner = fileInfo.owner().toUtf8();
    std::memcpy(&header.uname, owner.data(), qMin<int>(owner.length(), sizeof header.uname - 1));
    const QByteArray &group = fileInfo.group().toUtf8();
    std::memcpy(&header.gname, group.data(), qMin<int>(group.length(), sizeof header.gname - 1));
    std::memset(&header.chksum, ' ', sizeof header.chksum);
    quint64 checksum = 0;
    for (size_t i = 0; i < sizeof header; ++i)
        checksum += reinterpret_cast<char *>(&header)[i];
    const QByteArray checksumString = QString::fromLatin1("%1").arg(checksum,
        sizeof header.chksum - 1, 8, QLatin1Char('0')).toLatin1();
    std::memcpy(&header.chksum, checksumString.data(), checksumString.length());
    header.chksum[sizeof header.chksum-1] = 0;

    // end of borrowed from .../qt-creator/src/plugins/remotelinux/tarpackagecreationstep.cpp

    QByteArray finalHeader(reinterpret_cast<char *>(&header), sizeof header);

    return finalHeader;
}
