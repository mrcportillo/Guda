QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = haartPuro
TEMPLATE = app


SOURCES += haartMain.cpp


#-------------------Librerías---------------------
INCLUDEPATH += "/usr/include/opencv"
INCLUDEPATH += "/usr/local/include/player-3.0"
LIBS += `pkg-config opencv --cflags --libs`

CONFIG += extserialport
#-------------------------------------------------
