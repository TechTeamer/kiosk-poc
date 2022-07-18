#-------------------------------------------------
#
# Project created by QtCreator 2018-01-08T16:54:17
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ReadCard
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

linux: LIBS += -ldl

INCLUDEPATH += "../../include"
