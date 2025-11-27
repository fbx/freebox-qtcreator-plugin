#pragma once

#include <utils/filepath.h>
#include <projectexplorer/project.h>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QFileInfo>

namespace Core { class IDocument; }

namespace Freebox::Internal {

typedef struct {
    Utils::FilePath fileNameRelative;
    Utils::FilePath filePath;
    QFileInfo fileInfo;
    QByteArray content;
    bool toCheck;
} FreeStorePackageFile;

class FreeStorePackager : public QObject
{
    Q_OBJECT

public:
    FreeStorePackager(ProjectExplorer::Project *fbxProject, const QString &outFileName);
    bool operator()();
    QString outFileName() { return m_outFileName; }

private:
    bool checkSavedFiles(QList<Core::IDocument *> *failedToClose);
    bool isToCheck(const Utils::FilePath &filePath);
    bool preparePackaging();
    bool checkPackage();

    ProjectExplorer::Project *m_project;
    QString m_outFileName;
    bool m_seenManifest;

    QMap<QString, FreeStorePackageFile> m_packages;
    QStringList m_fileNameExists;

    Utils::FilePath m_projectDirPath;
    QList<Core::IDocument *> m_failedToClose;
};

} // namespace Freebox::Internal

