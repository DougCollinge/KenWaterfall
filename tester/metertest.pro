unix:!mac {
QWT_INCL_PATH = /home/ken/Projects/C++/qwt/src
QWT_LIB_PATH = /home/ken/Projects/C++/qwt/lib
}
win32: {
QWT_INCL_PATH = C:/qwt/qwt-6.1.3/src
QWT_LIB_PATH  = C:/qwt/qwt-6.1.3/lib
}
TARGET      = metertest
TEMPLATE    = app
target.path = ./bin

QT      += widgets multimedia
CONFIG  += qt c++11 thread exceptions silent debug

MOC_DIR     = .moc
OBJECTS_DIR = .obj
UI_DIR      = .ui

win32: {
INCLUDEPATH += $$FFTW3_INCL_PATH
INCLUDEPATH += $$QWT_INCL_PATH
}

INCLUDEPATH += \
    ../inc \
    ../inc/fft \
    ../inc/sound \
    ../inc/widgets 

HEADERS += ../inc/globals.hpp

HEADERS +=  \
    ../inc/sound/bufferobject.hpp \
    ../inc/sound/audioio.hpp

HEADERS += ../inc/widgets/vumeter.hpp

HEADERS += mainwindow.hpp

SOURCES += ../widgets/vumeter.cpp

SOURCES += \
    ../sound/bufferobject.cpp \
    ../sound/audioio.cpp

SOURCES += \
    mainwindow.cpp \
    main.cpp

FORMS   += mainwindow.ui

LIBS += -L$${QWT_LIB_PATH}/ -lqwt

win32: {
LIBS += -L$${FFTW3_LIB_PATH}/libfftw3-3.dll
LIBS += -L$${FFTW3_LIB_PATH}/libfftw3l-3.dll
LIBS += -L$${FFTW3_LIB_PATH}/libfftw3f-3.dll
}

unix:!mac {
LIBS += -lfftw3
}
