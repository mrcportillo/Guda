QT += core
QT -= gui

CONFIG += c++11

CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp
INCLUDEPATH += "/usr/include/opencv"

LIBS += `pkg-config opencv --cflags --libs`
