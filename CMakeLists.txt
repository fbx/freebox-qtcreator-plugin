cmake_minimum_required(VERSION 3.10)

# Remove when sharing with others.
list(APPEND CMAKE_PREFIX_PATH "/home/rawoul/Qt/Tools/QtCreator")

project(Freebox)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 17)

find_package(QtCreator REQUIRED COMPONENTS Core)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
set(QtX Qt${QT_VERSION_MAJOR})

add_qtc_plugin(Freebox
  PLUGIN_DEPENDS
    QtCreator::Core
    QtCreator::ProjectExplorer
    QtCreator::QtSupport
  DEPENDS
    ${QtX}::Widgets
    QtCreator::ExtensionSystem
    QtCreator::Utils
  SOURCES
    .github/workflows/build_cmake.yml
    .github/workflows/README.md
    README.md
    freebox.qrc
    freebox.cpp
    freebox.h
    freeboxconfiguration.cpp
    freeboxconfiguration.h
    freeboxdevice.cpp
    freeboxdevice.h
    freeboxruncontrol.cpp
    freeboxruncontrol.h
    freeboxsignaloperation.cpp
    freeboxsignaloperation.h
    freebox_global.h
    freeboxconstants.h
    http/client.cpp
    http/client.h
    http/constants.h
    http/http.cpp
    http/http.h
    http/server.cpp
    http/server.h
    remote/remoteconstants.h
    remote/remoteqml.cpp
    remote/remoteqml.h
    ssdp/ssdpclient.cpp
    ssdp/ssdpclient.h
    ssdp/ssdpconstants.h
    ssdp/ssdpmessage.cpp
    ssdp/ssdpmessage.h
    ssdp/ssdpsearch.cpp
    ssdp/ssdpsearch.h
    ssdp/udplistener.cpp
    ssdp/udplistener.h
    ssdp/udpsocket.cpp
    ssdp/udpsocket.h
    util/jsonrpc.cpp
    util/jsonrpc.h
)
