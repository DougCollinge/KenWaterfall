include(ardop.pri)
TEMPLATE = app
debug:TARGET = ardopd
release:TARGET = ardop
DESTDIR = bin
DESTDIR_TARGET = bin

QT     += core gui widgets multimedia network serialport
CONFIG += qt resources thread C++11 silent
CONFIG += c++11

debug:CONFIG += exceptions debug

MOC_DIR     = .moc
RCC_DIR     = .rcc
OBJECTS_DIR = .obj
UI_DIR      = .ui
UI_HEADERS_DIR = .ui

INCLUDEPATH += \
    images \
    inc \
    inc/Utils \
    inc/Sound \
    inc/Widgets

INCLUDEPATH += C:\\Users\\dcolling\\install\\fftw-3.3.4-32

win32: {
INCLUDEPATH += C:\\Users\\dcolling\\install\\fftw-3.3.4-32
#INCLUDEPATH += ../../install/fftw-3.3.4-64
}

win32: {
DEPENDPATH  += C:\\Users\\dcolling\\install\\fftw-3.3.4-32
}

HEADERS += inc/Utils/utils.hpp

HEADERS += \
    inc/Sound/audioinputio.hpp \
    inc/Sound/bufferobject.hpp

HEADERS += \
    inc/Widgets/waterfallwidget.hpp \
    inc/Widgets/vumeterwidget.hpp

HEADERS += \
    inc/tncpanel.hpp \
    inc/tncsetup.hpp

HEADERS += \
    inc/ardop_global.hpp \
    inc/modem.hpp


SOURCES += \
    sound/bufferobject.cpp \
    sound/audioinputio.cpp


SOURCES += \
    widgets/waterfallwidget.cpp \
    widgets/vumeterwidget.cpp

SOURCES += utils/utils.cpp

SOURCES += \
    tncpanel.cpp \
    tncsetup.cpp \
    modem.cpp \
    main.cpp

FORMS += \
    ui/tncpanel.ui \
    ui/tncsetup.ui

RESOURCES += \
    images/ardopres.qrc


LIBS += -LC:\\Users\\dcolling\\install\\fftw-3.3.4-32 -lfftw3-3
