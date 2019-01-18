include(../../gtest.pri)
include(../../gmock.pri)

TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    test.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../mylib/release/ -lmylib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../mylib/debug/ -lmylib

INCLUDEPATH += $$PWD/../mylib
DEPENDPATH += $$PWD/../mylib
