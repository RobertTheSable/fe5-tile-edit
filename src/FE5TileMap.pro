#-------------------------------------------------
#
# Project created by QtCreator 2019-04-18T20:10:57
#
#-------------------------------------------------

QT       += core gui
CONFIG   += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FE5TileMap
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
        fecompress.cpp \
        tilesetlabel.cpp \
        bgtileset.cpp \
        addresser/addresser.cpp \
    exportbindialog.cpp \
    opendirdialog.cpp

HEADERS += \
    fecompress.h \
    tilesetlabel.h \
    bgtileset.h \
    mainwindow.h \
    addresser/addresser.h \
    exportbindialog.h \
    undostack.h \
    opendirdialog.h

FORMS += \
        mainwindow.ui \
    exportbindialog.ui \
    opendirdialog.ui

win32: {
    ini_cpy.commands = $$quote($(COPY_DIR) $$shell_path($$PWD/ini) $$OUT_PWD/$(DESTDIR))
    QMAKE_EXTRA_TARGETS += ini_cpy
}

unix:!macx {
    target.path += /usr/local/bin
    ini.path    = /user/local/share/$${TARGET}/ini
    ini.files   = ini/*
    INSTALLS    += ini target
    ini_cpy.commands = $$quote($(COPY_DIR) $$shell_path($$PWD/ini) $$OUT_PWD/$(DESTDIR))
    QMAKE_EXTRA_TARGETS += ini_cpy
}

macx: {
    ini.path    = Contents/Resources
    ini.files   = ini/
    QMAKE_BUNDLE_DATA += ini
}
