TARGET      = testpanel
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
    ../inc/Sound \
    ../inc/Protocol \
    ../inc/Utils \
    ../inc/Widgets

HEADERS += ../inc/globals.hpp

#HEADERS += ../inc/FFT/fftcalc.hpp

HEADERS += ../inc/ardopmodem.hpp

HEADERS +=  \
    ../inc/Sound/bufferobject.hpp \
    ../inc/Sound/audioio.hpp

HEADERS += ../inc/Utils/utils.hpp

#HEADERS += \
#    ../inc/Protocol/ardopprotocol.hpp \
#    ../inc/Protocol/reedsolomon.hpp \
#    ../inc/Protocol/frameinfo.hpp

#HEADERS += \
#    ../inc/Widgets/waterfallwidget.hpp \
#    ../inc/Widgets/vumeterwidget.hpp

HEADERS += \
#    ../inc/demodulator.hpp \
#    ../inc/modulator.hpp \
#    ../inc/radiosetup.hpp \
#    ../inc/radio.hpp \
    ../inc/tncpanel.hpp

HEADERS += \
    channel.hpp \
    testpanel.hpp

SOURCES += \
    ../sound/bufferobject.cpp \
    ../sound/audioio.cpp

SOURCES += ../hostinterface.cpp

SOURCES += ../utils/utils.cpp

#SOURCES += \
#    ../protocol/ardopprotocol.cpp \
#    ../protocol/reedsolomon.cpp \
#    ../protocol/frameinfo.cpp

SOURCES += \
#    ../demodulator.cpp \
#    ../modulator.cpp \
#    ../radiosetup.cpp \
#    ../radio.cpp \
    ../tncpanel.cpp

SOURCES += \
    channel.cpp \
    testpanel.cpp \
    main.cpp

FORMS   += \
    ../ui/tncpanel.ui \
    testpanel.ui

