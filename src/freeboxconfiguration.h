#pragma once

#include <QObject>
#include <QSet>
#include <QNetworkInterface>
#include <QHostAddress>

namespace Freebox {

namespace Ssdp {
class Client;
class Message;
}

class FreeboxDevice;

namespace Internal {

class FreeboxPlugin;

class FreeboxConfiguration : public QObject
{
    friend class FreeboxPlugin;

    Q_OBJECT

public:
    static void updateKits();
    static FreeboxConfiguration *instance();

public slots:
    void updateDevices();
    void filterMessage(const QHostAddress &addr,
                       const Ssdp::Message &message);
    void freeboxAdd(const QHostAddress &addr,
                    const QString &usn);
    void freeboxDel(const QString &usn);

private:
    friend void setupFreeboxConfiguration();
    FreeboxConfiguration();

    Ssdp::Client *mSSDPClient;
    QSet<QString> mFreebox;
};

void setupFreeboxConfiguration();

} // namespace Internal
} // namespace Freebox
