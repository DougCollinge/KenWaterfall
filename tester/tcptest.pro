TARGET      = tcptestui
TEMPLATE    = app
target.path = ./bin

QT      += widgets multimedia network serialport
CONFIG  += qt c++11 thread exceptions silent debug

MOC_DIR     = .moc
OBJECTS_DIR = .obj
UI_DIR      = .ui


INCLUDEPATH += \
    ../inc \
#    ../inc/FFT \
    ../inc/Network \
#    ../inc/Sound \
#    ../inc/Protocol \
    ../inc/Utils \
    ../inc/Widgets

HEADERS += ../inc/globals.hpp

HEADERS += ../inc/ardopmodem.hpp

HEADERS += ../inc/Utils/utils.hpp

HEADERS += \
    ../inc/tncpanel.hpp

HEADERS += tcptestui.cpp

SOURCES += ../utils/utils.cpp

SOURCES += \
    ../src/ardopmodem.cpp \
    ../src/tncpanel.cpp

SOURCES += \
    main.cpp

FORMS += \
    ../ui/tncpanel.ui
