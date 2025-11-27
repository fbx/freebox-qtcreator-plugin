#include "freestorepackager.h"
#include "fileformat/manifest.h"
#include "util/gzipper.h"
#include "util/tar.h"

#include <coreplugin/documentmanager.h>
#include <coreplugin/icore.h>
#include <coreplugin/idocument.h>
#include <qmljs/qmljsmodelmanagerinterface.h>

#include <utils/filepath.h>
#include <utils/filestreamer.h>
#include <utils/filestreamermanager.h>
#include <utils/fileutils.h>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QSet>
#include <QStringList>

using namespace Utils;

namespace Freebox::Internal {
//Remplacer fbxProject par QmlProject

FreeStorePackager::FreeStorePackager(ProjectExplorer::Project *fbxProject, const QString &outFileName)
    : m_project(fbxProject)
    , m_outFileName(outFileName)
    , m_seenManifest(false)
{}

bool FreeStorePackager::checkSavedFiles(QList<Core::IDocument *> *failedToClose)
{
    bool cancelledSave = false;
    bool alwaysSave = false;

    Core::DocumentManager::saveAllModifiedDocuments(
        QString(), &cancelledSave, QString(), &alwaysSave, failedToClose);

    return !cancelledSave;
}

bool FreeStorePackager::isToCheck(const Utils::FilePath &filePath)
{
    for (int i = 0; i < m_failedToClose.size(); ++i) {
        Core::IDocument *id = m_failedToClose.at(i);
        if (filePath == id->filePath()) {
            m_failedToClose.removeAt(i);
            return false;
        }
    }
    return true;
}

bool FreeStorePackager::preparePackaging()
{
    m_projectDirPath = m_project->projectDirectory();
    FilePaths files = m_project->files(ProjectExplorer::Project::AllFiles);

    QSet<QString> seenDirs;

    for (const Utils::FilePath &filePath : std::as_const(files)) {
        if (!filePath.exists()) {
            QMessageBox::critical(
                Core::ICore::mainWindow(),
                tr("Make FreeStore Package Unsuccessful"),
                tr("File not found: '%1'").arg(filePath.toUserOutput()));
            return false;
        }

        if (filePath == m_projectDirPath)
            continue;

        const Utils::FilePath relPath = filePath.relativeChildPath(m_projectDirPath);
        const QString relKey = relPath.path();
        if (relKey.isEmpty())
            continue;

        const Utils::FilePath parentDir = filePath.parentDir();
        const QString relDir = parentDir.relativeChildPath(m_projectDirPath).path();
        if (!relDir.isEmpty() && !seenDirs.contains(relDir)) {
            seenDirs.insert(relDir);
            FreeStorePackageFile pfDir;
            pfDir.fileNameRelative = parentDir.relativeChildPath(m_projectDirPath);
            pfDir.filePath = parentDir;
            pfDir.fileInfo = parentDir.toFileInfo();
            pfDir.toCheck = false;
            m_packages.insert(relDir, pfDir);
        }

        FreeStorePackageFile pf;
        pf.fileInfo = filePath.toFileInfo();
        pf.fileNameRelative = relPath;
        pf.filePath = filePath;
        pf.toCheck = isToCheck(filePath);

        QFile file(filePath.toFSPathString());
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(
                Core::ICore::mainWindow(),
                tr("Make FreeStore Package Unsuccessful"),
                tr("Cannot open file '%1' for reading.").arg(filePath.toUserOutput()));
            return false;
        }
        pf.content = file.readAll();
        file.close();

        m_packages.insert(relKey, pf);
        m_fileNameExists.append(pf.fileNameRelative.fileName());
    }

    return true;
}

bool FreeStorePackager::checkPackage()
{
    for (const FreeStorePackageFile &pf : m_packages.values()) {
        if (!pf.toCheck)
            continue;

        QmlJS::Dialect dialect = QmlJS::ModelManagerInterface::guessLanguageOfFile(pf.filePath);
        QmlJS::Document::MutablePtr doc = QmlJS::Document::create(pf.filePath, dialect);

        bool checkingError = false;
        QString checkingErrorStr(tr("Syntax Error"));

        if (dialect.dialect() == QmlJS::Dialect::Json) {
            QJsonParseError jsError;
            const QJsonDocument jsonDoc = QJsonDocument::fromJson(pf.content, &jsError);

            if ((checkingError = jsonDoc.isNull()))
                checkingErrorStr = jsError.errorString();
            else if (pf.fileNameRelative.toFSPathString() == QLatin1String("manifest.json")) {
                m_seenManifest = true;
                Freebox::Fileformat::Manifest man(jsonDoc);
                if ((checkingError = !man.isValid()))
                    checkingErrorStr = man.lastError();
                else {
                    QStringList requiredFiles = man.files();
                    QStringList missingFiles;
                    for (quint16 lo = 0; lo < requiredFiles.size(); lo++)
                        if (!m_fileNameExists.contains(requiredFiles[lo]))
                            missingFiles.append(requiredFiles[lo]);

                    if (!missingFiles.isEmpty()) {
                        checkingError = true;
                        checkingErrorStr = tr("The following files are missing or mispelled:");
                        for (quint16 lo = 0; lo < missingFiles.size(); lo++)
                            checkingErrorStr += tr("<br>%1").arg(missingFiles[lo]);
                    }
                }
            }
        } else if (dialect.isFullySupportedLanguage()) {
            doc->setSource(QString::fromUtf8(pf.content));

            switch (dialect.dialect()) {
            case QmlJS::Dialect::Qml:
            case QmlJS::Dialect::QmlQtQuick2:
            case QmlJS::Dialect::QmlQtQuick2Ui:
                if (!doc->parseQml())
                    checkingError = true;
                break;
            case QmlJS::Dialect::JavaScript:
                if (!doc->parseJavaScript())
                    checkingError = true;
                break;
            default:
                break;
            }
        }

        if (checkingError) {
            QMessageBox::critical(
                Core::ICore::mainWindow(),
                tr("Make FreeStore Package Unsuccessful"),
                tr("Error checking file '%1': %2")
                    .arg(pf.fileNameRelative.toFSPathString())
                    .arg(checkingErrorStr));
            return false;
        }
    }

    if (!m_seenManifest) {
        QMessageBox::critical(
            Core::ICore::mainWindow(),
            tr("Make FreeStore Package Unsuccessful"),
            tr("Missing file 'manifest.json'"));
        return false;
    }

    return true;
}

bool FreeStorePackager::operator()()
{
    MAN_ASSERT(checkSavedFiles(&m_failedToClose), return false);

    MAN_ASSERT(preparePackaging(), return false);

    MAN_ASSERT(checkPackage(), return false);

    Tar tar;
    QByteArray ba = tar.package(m_packages);
    QString outFileName = QDir::toNativeSeparators(m_outFileName);

    QFile outFile(outFileName);
    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QMessageBox::critical(
            Core::ICore::mainWindow(),
            tr("Make FreeStore Package Unsuccessful"),
            tr("Cannot open file '%1' for writing.").arg(outFileName));
        return false;
    }

    if (!outFile.write(Gzipper::gzip(ba))) {
        QMessageBox::critical(
            Core::ICore::mainWindow(),
            tr("Make FreeStore Package Unsuccessful"),
            tr("Cannot write to file '%1'.").arg(outFileName));
        outFile.close();
        return false;
    }

    outFile.close();

    return true;
}

} // namespace Freebox::Internal
