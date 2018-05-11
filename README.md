# Freebox plugin for QtCreator

## Features

This plugin helps QML application development for Freebox:

* It declares some wizards for new projects;

* It handles `.fbxproject` files;

* It discovers Freebox with developer mode active, and allows remote
  debugging on them.

## Building on Linux

Qt-5.8 is required (binary distribution for developer is enough).

You'll need a QtCreator source tree, version **v4.3.0**, and
corresponding build tree.  Clone from QtCreator's Git
following [Qt's instructions](https://wiki.qt.io/Building_Qt_Creator_from_Git)
and checkout the tag **v4.3.0** before to build.

After build of QtCreator v4.3.0, prepare your environment to actually
point to source and build directories:
```
$ export QTC_SOURCE=$PWD/qt-creator-v4.3.0
$ export QTC_BUILD=$PWD/qt-creator-v4.3.0-build
```

Checkout and build this plugin:
```
$ git clone https://github.com/fbx/freebox-qtcreator-plugin.git
$ mkdir freebox-qtcreator-plugin-build
$ cd freebox-qtcreator-plugin-build
$ $PATH_TO_QT5/bin/qmake ../freebox-qtcreator-plugin
$ make -j10
```

The plugin will automatically be installed at the correct location.

## Support

* [Issue tracker on Github](https://github.com/fbx/freebox-qtcreator-plugin/issues)
* `sdk @ freebox.fr`
