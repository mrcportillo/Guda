QT += core
QT -= gui

CONFIG += c++11

TARGET = mhi
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

#-------------------Librerías---------------------
INCLUDEPATH += "/usr/include/opencv"
INCLUDEPATH += "/usr/local/include/player-3.0"
LIBS += `pkg-config opencv --cflags --libs`

CONFIG += extserialport
#-------------------------------------------------
