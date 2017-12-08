#-------------------------------------------------
#
# Project created by QtCreator 2017-12-07T10:01:50
#
#-------------------------------------------------

QT       += core gui
QT       += websockets serialport bluetooth

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EspruinoHost
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
    sslserver.cpp \
    websocketclient.cpp \
    blefinder.cpp \
    bleuart.cpp \
    app.cpp

HEADERS += \
    mainwindow.h \
    sslserver.h \
    websocketclient.h \
    blefinder.h \
    bleuart.h \
    app.h

FORMS += \
    mainwindow.ui

DISTFILES += \
    README.md \
    localhost.key \
    localhost.cert

# For windows, we'll have a resource file with the icon in
#RC_FILE = resources/app.rc
