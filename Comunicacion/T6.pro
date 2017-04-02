#-------------------------------------------------
#
# Project created by QtCreator 2015-06-22T22:19:23
#
#-------------------------------------------------

QT       += core gui serialport

TARGET = T6
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h\
           procimage.h


FORMS    += mainwindow.ui


#-------------------Librerías---------------------
INCLUDEPATH += "/usr/include/opencv"
INCLUDEPATH += "/usr/local/include/player-3.0"
LIBS += `pkg-config opencv --cflags --libs`

CONFIG += extserialport
#-------------------------------------------------

 QT += widgets
