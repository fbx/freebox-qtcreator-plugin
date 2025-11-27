#pragma once

#include <projectexplorer/target.h>
#include <solutions/tasking/tasktree.h>
#include <solutions/tasking/tasktreerunner.h>

#include "http/server.h"
#include "remote/remoteqml.h"

#include <QEventLoop>
#include <QTcpSocket>

namespace ProjectExplorer {
class RunControl;
}
namespace Utils {
class Port;
}

namespace Freebox::Internal {

class FreeboxRunnerInterface : public QObject
{
    Q_OBJECT

public:
    FreeboxRunnerInterface();
    // Gui init setters
    void setRunControl(ProjectExplorer::RunControl *runControl) { m_runControl = runControl; }
    void setHostAddress(const QHostAddress &address) { m_address = address; }
    void setServerPort(quint16 port) { m_serverport = port; }
    void enabledebug(bool enable) { m_debug = enable; }
    void setEntryPoint(const QString &entryPoint) { m_entryPoint = entryPoint; }

    ProjectExplorer::RunControl *runControl() const { return m_runControl; }
    Remote::QmlRemote *qmlRemote() { return &m_qmlRemote; }
    Http::Server *server() { return &m_server; }
    quint16 serverPort() const { return m_serverport; }
    bool debugon() const { return m_debug; }
    QString entryPoint() const { return m_entryPoint; }
    QTcpSocket *TCPstdout() { return &m_stdout; }
    QTcpSocket *TCPstderr() { return &m_stderr; }
    QHostAddress address() const { return m_address; }
    QEventLoop *getLoop() { return &loop; }

    void disconnectFromServer();

    // GUI -> business logic
    void cancel();

signals:
    void remoteStarted(const QUrl &serverUrl);
    void remoteStopped();

    // GUI -> business logic
    void canceled();

    // business logic -> GUI
    void started();
    void finished(const QString &errorMessage);

private:
    ProjectExplorer::RunControl *m_runControl = nullptr;
    QHostAddress m_address;
    Http::Server m_server;
    Remote::QmlRemote m_qmlRemote;
    QTcpSocket m_stdout;
    QTcpSocket m_stderr;
    quint16 m_serverport = 8234;
    bool m_debug = false;
    QString m_entryPoint = "main";
    QEventLoop loop;
    bool m_wasCancelled = false;
};

Tasking::ExecutableItem runnerRecipe(
    ProjectExplorer::RunControl *runControl,
    const Tasking::Storage<FreeboxRunnerInterface> &storage);

class FreeboxRunnerInterfaceTaskAdapter final
{
public:
    ~FreeboxRunnerInterfaceTaskAdapter()
    {
        if (m_task)
            m_task->disconnectFromServer();
    }

    void operator()(FreeboxRunnerInterface *task, Tasking::TaskInterface *iface)
    {
        m_task = task;

        QObject::connect(
            task,
            &FreeboxRunnerInterface::canceled,
            iface,
            [iface] { iface->reportDone(Tasking::DoneResult::Success); },
            Qt::SingleShotConnection);
    }

private:
    FreeboxRunnerInterface *m_task = nullptr;
};

using FreeboxRunnerTask
    = Tasking::CustomTask<FreeboxRunnerInterface, FreeboxRunnerInterfaceTaskAdapter>;

} // namespace Freebox::Internal
