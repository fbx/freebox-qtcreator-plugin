#include "freeboxrunner.h"
#include "freeboxrunnerworker.h"

#include "freeboxconstants.h"
#include "freeboxdevice.h"

#include "fileformat/manifest.h"
#include "http/server.h"
#include "remote/remoteqml.h"

#include <projectexplorer/project.h>
#include <projectexplorer/projectexplorerconstants.h>

#include <projectexplorer/devicesupport/devicekitaspects.h>
#include <projectexplorer/devicesupport/devicemanager.h>
#include <projectexplorer/environmentkitaspect.h>
#include <projectexplorer/qmldebugcommandlinearguments.h>
#include <projectexplorer/runcontrol.h>
#include <projectexplorer/target.h>

#include <debugger/debuggerkitaspect.h>
#include <debugger/debuggerrunconfigurationaspect.h>
#include <debugger/debuggerruncontrol.h>

#include <qmlprojectmanager/qmlprojectconstants.h>

#include <QTcpSocket>
#include <qtsupport/baseqtversion.h>
#include <qtsupport/qtkitaspect.h>

#include <utils/qtcprocess.h>
#include <utils/url.h>
#include <utils/utilsicons.h>

using namespace Debugger;
using namespace ProjectExplorer;
using namespace Utils;
using namespace Tasking;

namespace Freebox::Internal {

static void handleFreeboxStartedOnDevice(
    FreeboxRunnerInterface &task,
    RunControl *runControl,
    const StoredBarrier &barrier,
    quint16 port,
    quint16 out,
    quint16 err)
{
    QUrl qmlChannel = runControl->qmlChannel();

    task.TCPstdout()->connectToHost(task.address(), out);
    task.TCPstderr()->connectToHost(task.address(), err);
    qmlChannel.setPort(port);
    qmlChannel.setHost(task.address().toString());
    qmlChannel.setScheme(Utils::urlTcpScheme());
    runControl->setQmlChannel(qmlChannel);

    emit task.started();
}

static Group freeboxToolKicker(
    const StoredBarrier &barrier,
    RunControl *runControl,
    const QmlDebugServicesPreset &debugPreset)
{
    Target *target = runControl->target();
    QTC_ASSERT(target, return {});

    Kit *kit = target->kit();
    QTC_ASSERT(kit, return {});
    runControl->setDisplayName("Running on Freebox");

    const auto SetupDevice = [runControl, target, kit, barrier, debugPreset](
                                 FreeboxRunnerInterface &task) -> Tasking::SetupResult {
        task.setRunControl(runControl);

        IDevice::ConstPtr device = RunDeviceKitAspect::device(kit);
        FreeboxDevice::ConstPtr fbxDevice = std::dynamic_pointer_cast<const FreeboxDevice>(device);
        if (!fbxDevice || fbxDevice->address().isNull()) {
            runControl->postMessage(
                "No Freebox device available. Make sure a Freebox in developer mode "
                "has been discovered on the network.",
                Utils::ErrorMessageFormat);
            return Tasking::SetupResult::StopWithError;
        }
            task.setHostAddress(fbxDevice->address());
            task.qmlRemote()->setAddress(fbxDevice->address());

        task.enabledebug(debugPreset == QmlDebuggerServices);

        Remote::QmlRemote *qmlRemote = task.qmlRemote();
        QTcpSocket *TCPstdout = task.TCPstdout();
        QTcpSocket *TCPstderr = task.TCPstderr();

        QString projectPath;
        if (Project *project = target->project()) {
            projectPath = project->projectDirectory().path();
        }
        if (!projectPath.isEmpty()) {
            task.server()->setPath(projectPath);

            // Choose the entry point to launch from the application manifest
            Freebox::Fileformat::Manifest manifest(projectPath + "/manifest.json");
            if (manifest.isValid() && !manifest.defaultEntryPoint().isEmpty())
                task.setEntryPoint(manifest.defaultEntryPoint());
        }

        QObject::connect(
            qmlRemote,
            &Remote::QmlRemote::started,
            runControl,
            [&task, runControl, barrier](quint16 port, quint16 out, quint16 err) {
                handleFreeboxStartedOnDevice(task, runControl, barrier, port, out, err);
            });

        QObject::connect(
            qmlRemote, &Remote::QmlRemote::failed, runControl, [runControl](const QString &err) {
                runControl->appendMessage(err, Utils::StdErrFormat);
                    if (runControl->isRunning())
                        runControl->initiateStop();
            });

        QObject::connect(TCPstdout, &QTcpSocket::readyRead, runControl, [runControl, &task] {
            QByteArray output = task.TCPstdout()->readAll();
            runControl->appendMessage(QString::fromUtf8(output), Utils::StdOutFormat);
        });

        QObject::connect(TCPstderr, &QTcpSocket::readyRead, runControl, [runControl, &task] {
            QByteArray output = task.TCPstderr()->readAll();
            runControl->appendMessage(QString::fromUtf8(output), Utils::StdErrFormat);
        });

        const bool reactToStop = (debugPreset != QmlProfilerServices);

        QObject::connect(
            &task,
            &FreeboxRunnerInterface::finished,
            runControl,
            [runControl, reactToStop](const QString &errorString) {
                runControl->postMessage(errorString, Utils::NormalMessageFormat);
                if (reactToStop && runControl->isRunning())
                        runControl->initiateStop();
            });

        QObject::connect(
            &task,
            &FreeboxRunnerInterface::remoteStopped,
            runControl,
            [runControl, reactToStop] {
                if (reactToStop && runControl->isRunning())
                        runControl->initiateStop();
            });

        QObject::connect(
            &task,
            &FreeboxRunnerInterface::started,
            barrier.activeStorage(),
            &Barrier::advance,
            Qt::QueuedConnection);

        if (reactToStop) {
            QObject::connect(
                runControl, &RunControl::canceled, &task, &FreeboxRunnerInterface::cancel);
        }

        if (!task.server()->listen(QHostAddress::Any, 0)) {
            QString err = "Failed to start local server: " + task.server()->errorString();
            runControl->appendMessage(err, Utils::ErrorMessageFormat);
            return SetupResult::StopWithError;
        }


        qmlRemote->start(task.entryPoint(), task.server()->serverPort(), task.debugon());

        return Tasking::SetupResult::Continue;
    };

    const auto onFreeboxSetupDone = [runControl](DoneWith result) {
        if (result == DoneWith::Success)
            runControl->postMessage(("Run ended."), NormalMessageFormat);
        else if (result == DoneWith::Error)
            runControl->postMessage(("Run ended with error."), ErrorMessageFormat);
        else
            runControl->postMessage(("Run canceled."), ErrorMessageFormat);
    };

    // clang-format off
    return Group {
        FreeboxRunnerTask(SetupDevice, onFreeboxSetupDone)
    };
    // clang-format on
}

static Group freeboxToolRecipe(
    RunControl *runControl,
    const QmlDebugServicesPreset &debugPreset = {},
    const std::optional<ExecutableItem> &afterStartedRecipe = {})
{
    const auto kicker = [runControl, debugPreset](const StoredBarrier &barrier) {
        return freeboxToolKicker(barrier, runControl, debugPreset);
    };
    const WorkflowPolicy policy = afterStartedRecipe ? WorkflowPolicy::StopOnSuccessOrError
                                                     : WorkflowPolicy::StopOnError;
    return When(kicker, policy) >> Do{afterStartedRecipe ? *afterStartedRecipe : Sync([runControl] {
               runControl->reportStarted();
           })};
}

static void parametersModifier(RunControl *runControl, DebuggerRunParameters &rp)
{
    QUrl qmlChannel = runControl->qmlChannel();

    rp.setDisplayName("Freebox Remote");
    rp.setContinueAfterAttach(true);
    rp.setStartMode(AttachToQmlServer);
    rp.setQmlServer(qmlChannel);
    rp.populateQmlFileFinder(runControl);
}

static Group debugRecipe(RunControl *runControl)
{
    DebuggerRunParameters rp = DebuggerRunParameters::fromRunControl(runControl);


    runControl->requestQmlChannel();


    const auto modifier = [runControl](DebuggerRunParameters &rp) {
        parametersModifier(runControl, rp);
    };


    return freeboxToolRecipe(runControl, QmlDebuggerServices, debuggerRecipe(runControl, rp, modifier));
}

class FreeboxRunWorkerFactory final : public RunWorkerFactory
{
public:
    FreeboxRunWorkerFactory()
    {
        setId("FreeboxRunWorkerFactory");
        setRecipeProducer([](RunControl *runControl) {
            FreeboxDevice::ConstPtr freeboxdevice = std::dynamic_pointer_cast<const FreeboxDevice>(
                runControl->device());
            runControl->setIcon(Icons::RUN_SMALL_TOOLBAR);
            runControl->setDisplayName(
                QString("Run on %1").arg(freeboxdevice ? freeboxdevice->displayName() : QString()));
            return freeboxToolRecipe(runControl);
        });
        addSupportedRunMode(ProjectExplorer::Constants::NORMAL_RUN_MODE);
        addSupportedRunConfig(Freebox::Constants::FREEBOX_RUNCONFIG_ID);
        addSupportedDeviceType(Freebox::Constants::FREEBOX_DEVICE_TYPE);
    }
};

class FreeboxDebugWorkerFactory final : public RunWorkerFactory
{
public:
    FreeboxDebugWorkerFactory()
    {
        setId("FreeboxDebugWorkerFactory");
        setRecipeProducer([](RunControl *runControl) { return debugRecipe(runControl); });
        addSupportedRunMode(ProjectExplorer::Constants::DEBUG_RUN_MODE);
        addSupportedRunConfig(Freebox::Constants::FREEBOX_RUNCONFIG_ID);
        addSupportedDeviceType(Freebox::Constants::FREEBOX_DEVICE_TYPE);
    }
};

class FreeboxQmlProfilerWorkerFactory final : public RunWorkerFactory
{
public:
    FreeboxQmlProfilerWorkerFactory()
    {
        setId("FreeboxQmlProfilerWorkerFactory");
        setRecipeProducer([](RunControl *runControl) {
            runControl->requestQmlChannel();
            return freeboxToolRecipe(
                runControl,
                {QmlProfilerServices},
                runControl->createRecipe(ProjectExplorer::Constants::QML_PROFILER_RUNNER));
        });
        addSupportedRunMode(ProjectExplorer::Constants::QML_PROFILER_RUN_MODE);
        addSupportedRunConfig(Constants::FREEBOX_RUNCONFIG_ID);
        addSupportedDeviceType(Freebox::Constants::FREEBOX_DEVICE_TYPE);
    }
};

void setupFreeboxRunWorker()
{
    static FreeboxRunWorkerFactory theRunWorkerFactory;
    static FreeboxDebugWorkerFactory theDebugWorkerFactory;
    static FreeboxQmlProfilerWorkerFactory theQmlProfilerWorkerFactory;
}

} // namespace Freebox::Internal
