CONFIG -= qt

TARGET = lib
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    SomeContainer.cpp

HEADERS += \
    SomeContainer.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
