#-------------------------------------------------
#
# Project created by QtCreator 2015-12-18T17:27:17
#
#-------------------------------------------------


QT       += core
QT       += network

QT       -= gui

LIBS += libCsta32.a
LIBS += libencrptinterface.a

TARGET = TsapiServer
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    CtiCtrl.cpp \
    mainentry.cpp \
    DataProc.cpp \
    CtiEvent.cpp \
    MiddleDll.cpp \
    FtpDll.cpp \
    ocxdll.cpp \
    cticlientmodleevent.cpp

HEADERS += \
    GlobalTsapiServer.h \
    Lock.h \
    CtiCtrl.h \
    mainentry.h \
    DataProc.h \
    CtiEvent.h \
    MiddleDll.h \
    FtpDll.h \
    ocxdll.h \
    cticlientmodleevent.h


