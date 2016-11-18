TEMPLATE = app
TARGET = tcptest
DESTDIR = ../../bin
DESTDIR_TARGET = ../../bin

QT     += core gui widgets network
CONFIG += qt thread exceptions quiet debug # resources

MOC_DIR     = .moc
RCC_DIR     = .rcc
OBJECTS_DIR = .obj
UI_DIR      = .ui
UI_HEADERS_DIR = .ui
UI_SOURCES_DIR = .

INCLUDEPATH += \
    ../../inc \
    ../../inc/Network \
    .

HEADERS += \
    ../../inc/Network/tcpserver.hpp \
    ../../inc/Network/connectionthread.hpp \
    ../../inc/Network/tcpconnection.hpp

HEADERS   += tcptest.hpp

SOURCES   += \
    ../../network/tcpserver.cpp \
    ../../network/connectionthread.cpp \
    ../../network/tcpconnection.cpp

SOURCES   += \
    tcptest.cpp \
    main.cpp


FORMS     += tcptest.ui

RESOURCES += \
#    tcptester.qrc \
    ../../images/ardopres.qrc
