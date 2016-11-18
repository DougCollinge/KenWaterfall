TEMPLATE = app
TARGET   = guitest
DESTDIR  = ../../bin
DESTDIR_TARGET = ../../bin

QT      += core gui widgets multimedia
CONFIG  += qt thread
CONFIG += c++11

debug:CONFIG += exceptions debug


win32: {
INCLUDEPATH += C:\\Users\\dcolling\\install\\fftw-3.3.4-32
}
INCLUDEPATH += \
    ../../inc \
    ../../inc/Sound \
    ../../inc/Utils \
    ../../inc/Widgets

win32: {
DEPENDPATH += C:\\FFTW3\
}

MOC_DIR  = .moc
RCC_DIR  = .rcc
OBJECTS_DIR = .obj
UI_DIR   = .ui
UI_HEADERS_DIR = .ui
UI_SOURCES_DIR = .

HEADERS += ../../inc/ardop_global.hpp

HEADERS += \
    ../../inc/Sound/audioinputio.hpp \
    ../../inc/Sound/bufferobject.hpp

HEADERS += \
    ../../inc/Utils/utils.hpp \
    ../../inc/Utils/fftw3wrapper.hpp

HEADERS += \
    ../../inc/Widgets/vumeterwidget.hpp \
    ../../inc/Widgets/fftdisplay.hpp \
    ../../inc/Widgets/markerwidget.hpp \
    ../../inc/Widgets/spectrumwidget.hpp

HEADERS += guitest.hpp


SOURCES += \
    ../../sound/audioinputio.cpp \
    ../../sound/bufferobject.cpp

SOURCES += \
    ../../utils/utils.cpp \
    ../../utils/fftw3wrapper.cpp

SOURCES += \
    ../../widgets/vumeterwidget.cpp \
    ../../widgets/fftdisplay.cpp \
    ../../widgets/markerwidget.cpp \
    ../../widgets/spectrumwidget.cpp

SOURCES += \
    main.cpp \
    guitest.cpp

FORMS   += \
    ../../ui/spectrumwidget.ui \
    guitest.ui

win32: {
LIBS += -LC:\\Users\\dcolling\\install\\fftw-3.3.4-32 -lfftw3-3
}

unix:!mac {
LIBS += -L/usr/lib/ -lfftw3
}

