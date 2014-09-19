#
# You must define QTC_SOURCE and QTC_BUILD to point to
# QtCreator source and build path, respectively.
QTCREATOR_SOURCES = $$(QTC_SOURCE)
IDE_BUILD_TREE = $$(QTC_BUILD)
isEmpty(QTCREATOR_SOURCES):error(QTC_SOURCE env variable not defined)
isEmpty(IDE_BUILD_TREE):error(QTC_BUILD env variable not defined)
USE_USER_DESTDIR = yes

win32 {
    DEFINES += uint16_t=quint16
}

# Sources

INCLUDEPATH += src .

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
    src/freestorepackager.cc \
    src/localrunconfiguration.cc \
    src/manager.cc \
    src/node.cc \
    src/processsignaloperation.cc \
    src/project.cc \
    src/remoterunconfiguration.cc \
    src/runconfigurationfactory.cc \
    src/runconfigurationwidget.cc \
    src/runcontrol.cc \
    src/runcontrolfactory.cc \
    src/tar.cc \
    src/wizard.cc \
    src/wizardpages.cc \
    ssdp/ssdpclient.cc \
    ssdp/ssdpmessage.cc \
    ssdp/ssdpsearch.cc \
    ssdp/udplistener.cc \
    ssdp/udpsocket.cc \
    util/crc32.c \
    util/gzipper.cc \
    util/jsonrpc.cc \
    fileformat/filefilteritems.cc \
    fileformat/qmlprojectfileformat.cpp \
    fileformat/qmlprojectitem.cpp \
    fileformat/manifest.cc \
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
    src/freestorepackager.hh \
    src/localrunconfiguration.hh \
    src/manager.hh \
    src/node.hh \
    src/processsignaloperation.hh \
    src/project.hh \
    src/remoterunconfiguration.hh \
    src/runconfigurationfactory.hh \
    src/runconfigurationwidget.hh \
    src/runcontrol.hh \
    src/runcontrolfactory.hh \
    src/tar.hh \
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
    util/crc32.h \
    util/gzipper.hh \
    util/jsonrpc.hh \
    fileformat/filefilteritems.hh \
    fileformat/qmlprojectfileformat.hh \
    fileformat/qmlprojectitem.hh \
    fileformat/manifest.hh

DEFINES += FREEBOX_LIBRARY

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
