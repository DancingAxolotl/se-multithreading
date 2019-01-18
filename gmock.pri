win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../gtest/googlemock/release/ -lgmock
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../gtest/googlemock/debug/ -lgmock

INCLUDEPATH += $$PWD/gtest/googlemock/include
INCLUDEPATH += $$PWD/gtest/googletest/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../gtest/googlemock/release/libgmock.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../gtest/googlemock/debug/libgmock.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../gtest/googlemock/release/gmock.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../gtest/googlemock/debug/gmock.lib
