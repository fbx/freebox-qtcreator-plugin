#
# You must define QTC_SOURCE and QTC_BUILD to point to
# QtCreator source and build path, respectively.
QTCREATOR_SOURCES = $$(QTC_SOURCE)
IDE_BUILD_TREE = $$(QTC_BUILD)
isEmpty(QTCREATOR_SOURCES):error(QTC_SOURCE env variable not defined)
isEmpty(IDE_BUILD_TREE):error(QTC_BUILD env variable not defined)
USE_USER_DESTDIR = yes

# Sources

INCLUDEPATH += src

SOURCES += \
    src/analyzer.cc \
    src/app.cc \
    src/configuration.cc \
    src/debugger.cc \
    src/device.cc \
    src/devicefactory.cc \
    src/environment.cc \
    src/file.cc \
    src/freeboxplugin.cc \
    src/localrunconfiguration.cc \
    src/manager.cc \
    src/manifest.cc \
    src/node.cc \
    src/processsignaloperation.cc \
    src/project.cc \
    src/remoterunconfiguration.cc \
    src/runconfigurationfactory.cc \
    src/runconfigurationwidget.cc \
    src/runcontrol.cc \
    src/runcontrolfactory.cc \
    src/wizard.cc \
    src/wizardpages.cc \
    ssdp/ssdpclient.cc \
    ssdp/ssdpmessage.cc \
    ssdp/ssdpsearch.cc \
    ssdp/udplistener.cc \
    ssdp/udpsocket.cc \
    util/jsonrpc.cc \
    fileformat/filefilteritems.cc \
    fileformat/qmlprojectfileformat.cpp \
    fileformat/qmlprojectitem.cpp \
    remote/remoteqml.cc \
    http/client.cc \
    http/http.cc \
    http/server.cc

HEADERS += \
    src/analyzer.hh \
    src/app.hh \
    src/configuration.hh \
    src/constants.hh \
    src/debugger.hh \
    src/device.hh \
    src/devicefactory.hh \
    src/environment.hh \
    src/file.hh \
    src/freebox_global.hh \
    src/freeboxplugin.hh \
    src/localrunconfiguration.hh \
    src/manager.hh \
    src/manifest.hh \
    src/node.hh \
    src/processsignaloperation.hh \
    src/project.hh \
    src/remoterunconfiguration.hh \
    src/runconfigurationfactory.hh \
    src/runconfigurationwidget.hh \
    src/runcontrol.hh \
    src/runcontrolfactory.hh \
    src/wizard.hh \
    src/wizardpages.hh \
    ssdp/ssdpclient.hh \
    ssdp/ssdpconstants.hh \
    ssdp/ssdpmessage.hh \
    ssdp/ssdpsearch.hh \
    ssdp/udplistener.hh \
    ssdp/udpsocket.hh \
    fileformat/manifest.hh \
    http/client.hh \
    http/constants.hh \
    http/http.hh \
    http/server.hh \
    remote/remoteconstants.hh \
    remote/remoteqml.hh \
    util/jsonrpc.hh \
    fileformat/filefilteritems.hh \
    fileformat/qmlprojectfileformat.hh \
    fileformat/qmlprojectitem.hh

DEFINES += FBXQML_LIBRARY

# Qt Creator linking

QTC_PLUGIN_NAME = Freebox

QTC_LIB_DEPENDS += \
    utils

QTC_PLUGIN_DEPENDS += \
    coreplugin \
    cpptools \
    qmakeprojectmanager \
    debugger \
    analyzerbase

QT += network

RESOURCES += freebox.qrc

include($$QTCREATOR_SOURCES/src/qtcreatorplugin.pri)

OTHER_FILES += Freebox.json

