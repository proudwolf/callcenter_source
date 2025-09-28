QT += core
QT -= gui
QT += network

LIBS += libencrptinterface.a

TARGET = AESServer
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    clientsock.cpp \
    dataproc.cpp \
    commonenginedll.cpp \
    ctictrl.cpp \
    urlconvert.cpp

HEADERS += \
    clientsock.h \
    globaleaesserver.h \
    dataproc.h \
    commonenginedll.h \
    ctictrl.h \
    Lock.h \
    urlconvert.h

