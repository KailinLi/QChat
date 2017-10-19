#-------------------------------------------------
#
# Project created by QtCreator 2017-10-19T16:05:20
#
#-------------------------------------------------

QT       += core gui
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QServer
TEMPLATE = app


SOURCES += main.cpp\
        qserver.cpp \
    message.cpp \
    tcpsocketmsg.cpp \
    parallelserver.cpp \
    connectthread.cpp

HEADERS  += qserver.h \
    message.h \
    tcpsocketmsg.h \
    parallelserver.h \
    connectthread.h

FORMS    += qserver.ui
