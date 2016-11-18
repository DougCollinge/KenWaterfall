unix:!mac {
QWT_INCL_PATH = /home/ken/Projects/C++/qwt/src
QWT_LIB_PATH = /home/ken/Projects/C++/qwt/lib
}
win32: {
QWT_INCL_PATH   = C:/qwt/qwt-6.1.3/src
QWT_LIB_PATH    = C:/qwt/qwt-6.1.3/lib
FFTW3_LIB_PATH  = C:/FFTW3
FFTW3_INCL_PATH = C:/FFTW3
}

TEMPLATE = app
TARGET   = wftest
DESTDIR  = bin
DESTDIR_TARGET = bin

VERSION = 0.5.0a
VER_MAJ = 0
VER_MIN = 5
VER_PAT = 0

QT      += widgets multimedia
CONFIG  += qt debug c++11 thread exceptions silent

MOC_DIR     = .moc
OBJECTS_DIR = .obj
UI_DIR      = .ui

win32: {
INCLUDEPATH += $$FFTW3_INCL_PATH
INCLUDEPATH += $$QWT_INCL_PATH
}

INCLUDEPATH += \
    ../inc \
    ../inc/FFT \
    ../inc/Sound \
    ../inc/Widgets

DEPENDPATH += \
    $$FFTW3_LIB_PATH \
    $$QWT_LIB_PATH

HEADERS += ../inc/globals.hpp

HEADERS +=  \
    ../inc/Sound/bufferobject.hpp \
    ../inc/Sound/audioio.hpp

HEADERS += \
    ../inc/Widgets/vumeterwidget.hpp \
    ../inc/Widgets/waterfallwidget.hpp

HEADERS += mainwindow.hpp

SOURCES += \
    ../widgets/vumeterwidget.cpp \
    ../widgets/waterfallwidget.cpp

SOURCES += \
    ../sound/bufferobject.cpp \
    ../sound/audioio.cpp

SOURCES += \
    mainwindow.cpp \
    main.cpp

FORMS   += mainwindow.ui

LIBS += -L$${QWT_LIB_PATH}/ -lqwt

win32: {
LIBS += -L$${FFTW3_LIB_PATH}/ -lfftw3-3
LIBS += -L$${FFTW3_LIB_PATH}/ -lfftw3l-3
LIBS += -L$${FFTW3_LIB_PATH}/ -lfftw3f-3
}

unix:!mac {
LIBS += -lfftw3
}
