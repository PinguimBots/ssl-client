TEMPLATE = app
TARGET = main
CONFIG *= console c++17 precompile_header link_pkgconfig
PKGCONFIG += protobuf

QT *= core network

SOURCES *= $$files($$PWD/src/*.cpp, true)
SOURCES *= $$files($$PWD/src/*.cc,  true)
SOURCES *= $$files($$PWD/src/*.c,   true)
HEADERS *= $$files($$PWD/src/*.hpp, true)
HEADERS *= $$files($$PWD/src/*.h,   true)
PRECOMPILED_HEADER = $$PWD/src/pch.h

DESTDIR     = $$system_path($$OUT_PWD/bin)
OBJECTS_DIR = $$system_path($$OUT_PWD/build)
MOC_DIR     = $$system_path($$OUT_PWD/build)
UI_DIR      = $$system_path($$OUT_PWD/build)

INCLUDEPATH += include src
LIBS += -lz
