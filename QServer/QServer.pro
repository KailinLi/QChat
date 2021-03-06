#-------------------------------------------------
#
# Project created by QtCreator 2017-10-19T16:05:20
#
#-------------------------------------------------

QT       += core gui
QT += network
QMAKE_CXXFLAGS_RELEASE -= -O3

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QServer
TEMPLATE = app


SOURCES += main.cpp\
        qserver.cpp \
    message.cpp \
    tcpsocketmsg.cpp \
    parallelserver.cpp \
    connectthread.cpp \
    connectthreadpool.cpp \
    userinfo.cpp \
    userinfolist.cpp \
    handledir.cpp

HEADERS  += qserver.h \
    message.h \
    tcpsocketmsg.h \
    parallelserver.h \
    connectthread.h \
    connectthreadpool.h \
    userinfo.h \
    userinfolist.h

FORMS    += qserver.ui

RESOURCES += \
    images.qrc
