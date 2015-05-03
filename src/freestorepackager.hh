#ifndef FREESTORE_PACKAGER_H
#define FREESTORE_PACKAGER_H

#include <QMap>
#include <QString>
#include <QStringList>
#include <QFileInfo>

namespace Core { class IDocument; }

namespace Freebox {

class Project;

namespace Internal {

typedef struct {
    QString fileNameRelative;
    QString filePath;
    QFileInfo fileInfo;
    QByteArray content;
    bool toCheck;
} FreeStorePackageFile;

class FreeStorePackager : public QObject
{
    Q_OBJECT

public:
    FreeStorePackager(Freebox::Project *fbxProject, const QString &outFileName);
    bool operator()();
    QString outFileName() { return m_outFileName; }

private:
    void initRegExp(char *key, char *regExp);
    bool checkSavedFiles(QList<Core::IDocument *> *failedToClose);
    bool isToCheck(const QString &filePath);
    bool preparePackaging();
    bool checkPackage();
    bool checkRequiredProperty(QJsonObject &obj, const char *key, const char *where);
    bool checkNumberProperty(QJsonObject &obj, const char *key, int min, int max, const char *where);
    bool checkRegExpProperty(QJsonObject &obj, QString &key, QString &where);
    bool checkStringProperty(QJsonObject &obj, const char *key, const char *regExp, const char *where);
    bool checkManifest(QJsonDocument &jsonDoc);

    Freebox::Project *m_project;
    QString m_outFileName;
    uint m_nbDefaults;
    bool m_seenManifest;

    QMap<int, FreeStorePackageFile> m_packages;
    QStringList m_fileNameExists;

    QString m_projectDirPath;
    QList<Core::IDocument *> m_failedToClose;

    QString m_lastCheckingErrorStr;
};

} // namespace Internal
} // namespace Freebox

#endif // FREESTORE_PACKAGER
