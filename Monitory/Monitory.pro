QT += core
QT -= gui

TARGET = Monitory
CONFIG += console
CONFIG -= app_bundle

QMAKE_CXXFLAGS += -std=c++11 #dodane przeze mnie

TEMPLATE = app

SOURCES += main.cpp \
    MonitorBuffer.cpp

HEADERS += \
    MonitorBuffer.hpp \
    monitor.h

