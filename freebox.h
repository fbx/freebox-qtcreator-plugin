#ifndef FREEBOXPLUGIN_H
#define FREEBOXPLUGIN_H

#include "freebox_global.h"

#include <extensionsystem/iplugin.h>

namespace Freebox {
namespace Internal {

class FreeboxPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "Freebox.json")

public:
    FreeboxPlugin();
    ~FreeboxPlugin() override;

    bool initialize(const QStringList &arguments, QString *errorString) override;
    void extensionsInitialized() override;
    ShutdownFlag aboutToShutdown() override;

private:
    void triggerAction();
};

} // namespace Internal
} // namespace Freebox

#endif // FREEBOXPLUGIN_H
