#-------------------------------------------------
#
# Project created by QtCreator 2016-06-25T22:46:02
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = histograma
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp\
        detectarvehiculo2.cpp

HEADERS  += mainwindow.h\
         detectarvehiculo2.h

FORMS    += mainwindow.ui

#-------------------Librerías---------------------
INCLUDEPATH += "/usr/include/opencv"
INCLUDEPATH += "/usr/local/include/player-3.0"
LIBS += `pkg-config opencv --cflags --libs`

CONFIG += extserialport
#-------------------------------------------------
