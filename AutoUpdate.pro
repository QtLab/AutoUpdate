#-------------------------------------------------
#
# Project created by QtCreator 2016-10-21T10:48:11
#
#-------------------------------------------------

QT       += core gui
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AutoUpdate
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp

HEADERS  += widget.h

FORMS    += widget.ui

RESOURCES += \
    res.qrc
RC_ICONS = images/logo.ico

RC_FILE = uac.rc

LIBS += -LC:/zlib-1.2.11 -lz
LIBS += -LC:/quazip-0.7.3/lib/ -lquazip
INCLUDEPATH += C:/quazip-0.7.3/quazip

