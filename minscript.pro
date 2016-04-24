TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS_WARN_OFF -= -Wunused-parameter

SOURCES += \
    minip.cpp \
    minipenv.cpp \
    minparser.cpp \
    minparsernodes.cpp \
    minpreproc.cpp \
    minscript.cpp \
    mintokenizer.cpp \
    osdep.cpp

include(deployment.pri)
#qtcAddDeployment()

HEADERS += \
    minhandle.h \
    minip.h \
    minipenv.h \
    minnativehlp.h \
    minparser.h \
    minparsernodes.h \
    minpreproc.h \
    mintoken.h \
    mintokenizer.h \
    module_test.h \
    osdep.h \
    platform.h

macx {
#    QMAKE_CXXFLAGS += -Wunused-parameter
}
