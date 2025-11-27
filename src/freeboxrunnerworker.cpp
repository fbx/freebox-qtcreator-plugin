#include "freeboxrunnerworker.h"
#include <projectexplorer/runcontrol.h>
#include <utils/qtcprocess.h>

using namespace ProjectExplorer;
using namespace Tasking;
using namespace Utils;

namespace Freebox::Internal {

FreeboxRunnerInterface::FreeboxRunnerInterface()
{
    connect(&m_stdout, &QTcpSocket::disconnected, this, [this] {
        if (m_stdout.state() != QAbstractSocket::UnconnectedState)
            m_stdout.disconnectFromHost();
        if (m_stderr.state() != QAbstractSocket::UnconnectedState)
            m_stderr.disconnectFromHost();

        emit remoteStopped();
    });
}

void FreeboxRunnerInterface::cancel()
{
    m_wasCancelled = true;
    emit canceled();
}

void FreeboxRunnerInterface::disconnectFromServer() {}

} // namespace Freebox::Internal
