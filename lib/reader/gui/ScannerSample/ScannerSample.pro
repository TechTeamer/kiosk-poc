#-------------------------------------------------
#
# Project created by QtCreator 2017-12-08T13:54:26
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ScannerSample
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += \
    mainwindow.h

FORMS    += mainwindow.ui

linux: LIBS += -ldl

INCLUDEPATH += "../../include"
