DEFINES += DEBUG_AUDIO
TEMPLATE = app
TARGET = audiotest
DESTDIR = ../../bin
DESTDIR_TARGET = ../../

QT     += core gui widgets multimedia
CONFIG += qt thread exceptions
debug:CONFIG += debug

MOC_DIR  = .moc
RCC_DIR  = .rcc
OBJECTS_DIR = .obj

INCLUDEPATH += \
    ../../inc \
    ../../inc/Sound \
    ../../inc/Widgets

HEADERS  += audiotest.hpp

HEADERS  += ../../inc/Widgets/vumeterwidget.hpp

HEADERS  += \
    ../../inc/Sound/bufferobject.hpp \
    ../../inc/Sound/audioio.hpp

SOURCES  += \
    ../../sound/bufferobject.cpp \
    ../../sound/audioio.cpp

SOURCES  += ../../widgets/vumeterwidget.cpp

SOURCES += \
    main.cpp \
    audiotest.cpp

FORMS    += audiotest.ui

RESOURCES += ../../images/ardopres.qrc

