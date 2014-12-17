#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include <coreplugin/icore.h>
#include <coreplugin/documentmanager.h>
#include <qmljs/qmljsmodelmanagerinterface.h>

#include "util/gzipper.hh"
#include "tar.hh"
#include "freestorepackager.hh"
#include "fileformat/manifest.hh"

namespace Freebox {
namespace Internal {

FreeStorePackager::FreeStorePackager(Freebox::Project *fbxProject, QString outFileName)
    : m_project(fbxProject),
      m_outFileName(outFileName),
      m_nbDefaults(0),
      m_seenManifest(false)
{
}

bool FreeStorePackager::checkSavedFiles(QList<Core::IDocument *> *failedToClose)
{
    bool cancelledSave = false;
    bool alwaysSave = false;

    Core::DocumentManager::saveAllModifiedDocuments(
        QString(), &cancelledSave,
        QString(), &alwaysSave,
        failedToClose);

    return !cancelledSave;
}

bool FreeStorePackager::isToCheck(QString filePath)
{
    foreach (Core::IDocument *id, m_failedToClose)
        if (id->filePath() == filePath) {
            m_failedToClose.removeOne(id);
            return false;
        }

    return true;
}

bool FreeStorePackager::preparePackaging()
{
    m_projectDirPath = m_project->projectDir().canonicalPath();
    QStringList paths = m_project->files();

    // Store file data and create virtual directories
    QMap<QString, bool> seenDirs;
    foreach (const QString &filePath, paths) {
        FreeStorePackageFile pf;

        pf.fileInfo = QFileInfo(filePath);
        const QString cPath = pf.fileInfo.canonicalFilePath();
        const QString cDir  = pf.fileInfo.canonicalPath();

        if (cDir != m_projectDirPath && !seenDirs.count(cDir)) {
            seenDirs[cDir] = true;
            FreeStorePackageFile pfDir;
            pfDir.fileNameRelative = cDir.mid(m_projectDirPath.length() + 1) + QChar::fromLatin1('/');
            pfDir.filePath = cDir;
            pfDir.fileInfo = QFileInfo(cDir);
            pfDir.toCheck = false; // virtual dirs are never parsed

            m_packages.insertMulti(cDir.length(), pfDir);
        }

        pf.fileNameRelative = cPath.mid(m_projectDirPath.length() + 1);
        pf.filePath = cPath;
        pf.toCheck = isToCheck(cPath);

        QFile file(cPath);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(Core::ICore::mainWindow(),
                                  tr("Make FreeStore Package Unsuccessful"),
                                  tr("Cannot open file '%1' for reading.")
                                  .arg(cPath));
            return false;
        }
        pf.content = file.readAll();
        file.close();

        m_packages.insertMulti(cPath.length(), pf);
        m_fileNameExists.append(pf.fileNameRelative);
    }

    return true;
}

bool FreeStorePackager::checkPackage()
{
    foreach (const FreeStorePackageFile &pf, m_packages.values()) {
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
            else if (pf.fileNameRelative == QLatin1String("manifest.json")) {
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
            case QmlJS::Dialect::QmlQtQuick1:
            case QmlJS::Dialect::QmlQtQuick2:
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
            QMessageBox::critical(Core::ICore::mainWindow(),
                                  tr("Make FreeStore Package Unsuccessful"),
                                  tr("Error checking file '%1': %2")
                                  .arg(pf.fileNameRelative)
                                  .arg(checkingErrorStr));
            return false;
        }
    }

    if (!m_seenManifest) {
        QMessageBox::critical(Core::ICore::mainWindow(),
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
        QMessageBox::critical(Core::ICore::mainWindow(),
                              tr("Make FreeStore Package Unsuccessful"),
                              tr("Cannot open file '%1' for writing.")
                              .arg(outFileName));
        return false;
    }

    if (!outFile.write(Gzipper::gzip(ba))) {
        QMessageBox::critical(Core::ICore::mainWindow(),
                              tr("Make FreeStore Package Unsuccessful"),
                              tr("Cannot write to file '%1'.")
                              .arg(outFileName));
        outFile.close();
        return false;
    }

    outFile.close();

    return true;
}

} // namespace Internal
} // namespace Freebox
