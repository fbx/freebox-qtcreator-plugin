cmake_minimum_required(VERSION 3.16)

if(APPLE)
    if(NOT CMAKE_OSX_ARCHITECTURES)
        set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64")
    endif()
endif()

if(DEFINED QTC_BUILD_PATH)
    message(STATUS "Using provided Qt Creator build path: ${QTC_BUILD_PATH}")
    list(APPEND CMAKE_PREFIX_PATH "${QTC_BUILD_PATH}")

else()
    # Probe the usual Qt Creator install locations. Override with
    #   -DQTC_BUILD_PATH=<path-to-qtcreator>  (handled above).
    set(_qtc_candidates
        "C:/Qt/Tools/QtCreator"                  # Windows, Qt online installer default
        "$ENV{USERPROFILE}/Qt/Tools/QtCreator"   # Windows, custom in-home install
        "$ENV{HOME}/Qt/Tools/QtCreator"          # Linux/macOS, Qt online installer
        "/opt/Qt/Tools/QtCreator")
    foreach(_cand IN LISTS _qtc_candidates)
        if(EXISTS "${_cand}")
            set(QT_CREATOR_PATH "${_cand}")
            break()
    endif()
    endforeach()

    if(QT_CREATOR_PATH)
        message(STATUS "Using detected Qt Creator path: ${QT_CREATOR_PATH}")
        list(APPEND CMAKE_PREFIX_PATH "${QT_CREATOR_PATH}")
    else()
        message(WARNING
            "Qt Creator dev package not found in the usual locations. "
            "Pass -DQTC_BUILD_PATH=<path-to-qtcreator> (e.g. C:/Qt/Tools/QtCreator).")
    endif()
endif()

message(STATUS "Qt Creator path: ${QT_CREATOR_PATH}")

project(Freebox)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(QtCreator REQUIRED COMPONENTS Core)
find_package(Qt6 COMPONENTS Widgets REQUIRED)

set(SRC_FILES
    src/freebox.cpp
    src/freebox_global.h
    src/freeboxconfiguration.cpp
    src/freeboxconfiguration.h
    src/freeboxconstants.h
    src/freeboxdevice.cpp
    src/freeboxdevice.h
    src/freeboxlog.h
    src/freeboxrunconfiguration.cpp
    src/freeboxrunconfiguration.h
    src/freeboxrunner.cpp
    src/freeboxrunner.h
    src/freeboxrunnerworker.cpp
    src/freeboxrunnerworker.h
    src/freeboxsignaloperation.cpp
    src/freeboxsignaloperation.h
    src/freestorepackager.cpp
    src/freestorepackager.h

    src/http/client.cpp
    src/http/client.h
    src/http/constants.h
    src/http/http.cpp
    src/http/http.h
    src/http/server.cpp
    src/http/server.h

    src/remote/remoteconstants.h
    src/remote/remoteqml.cpp
    src/remote/remoteqml.h

    src/ssdp/ssdpclient.cpp
    src/ssdp/ssdpclient.h
    src/ssdp/ssdpconstants.h
    src/ssdp/ssdpmessage.cpp
    src/ssdp/ssdpmessage.h
    src/ssdp/ssdpsearch.cpp
    src/ssdp/ssdpsearch.h
    src/ssdp/udplistener.cpp
    src/ssdp/udplistener.h
    src/ssdp/udpsocket.cpp
    src/ssdp/udpsocket.h

    src/util/crc32.c
    src/util/crc32.h
    src/util/gzipper.cpp
    src/util/gzipper.h
    src/util/jsonrpc.cpp
    src/util/jsonrpc.h
    src/util/tar.cpp
    src/util/tar.h

    src/fileformat/manifest.cpp
    src/fileformat/manifest.h

    Freebox.json.in)

add_qtc_plugin(Freebox
  PLUGIN_DEPENDS
    QtCreator::Core
    QtCreator::ProjectExplorer
    QtCreator::QmlProjectManager
    QtCreator::QtSupport
    QtCreator::Debugger
  DEPENDS
    Qt::Widgets
    QtCreator::ExtensionSystem
    QtCreator::QmlDebug
    QtCreator::Utils

  SOURCES
    README.md
    freebox.qrc
    ${SRC_FILES}
  DEFINES
    FREEBOX_LIBRARY
)

# Enable the Run button in Qt Creator
get_target_property(QtCreatorCorePath QtCreator::Core LOCATION)
find_program(QtCreatorExecutable
  NAMES
    qtcreator "Qt Creator"
  PATHS
    "${QtCreatorCorePath}/../../../bin"
    "${QtCreatorCorePath}/../../../MacOS"
  NO_DEFAULT_PATH
)
if (QtCreatorExecutable)
  add_custom_target(RunQtCreator
    COMMAND ${QtCreatorExecutable} -pluginpath $<TARGET_FILE_DIR:Freebox>
    DEPENDS Freebox
  )
  set_target_properties(RunQtCreator PROPERTIES FOLDER "qtc_runnable")
endif()
