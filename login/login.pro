#-------------------------------------------------
#
# Project created by QtCreator 2016-05-06T12:09:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = login
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    logindlg.cpp

HEADERS  += mainwindow.h \
    logindlg.h

FORMS    += mainwindow.ui

RESOURCES += \
    vpn.qrc
