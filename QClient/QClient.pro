#-------------------------------------------------
#
# Project created by QtCreator 2017-10-19T16:04:15
#
#-------------------------------------------------

QT       += core gui
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QClient
TEMPLATE = app


SOURCES += main.cpp\
        qclient.cpp \
    signin.cpp \
    message.cpp \
    tcpsocketmsg.cpp \
    signup.cpp \
    connectthread.cpp \
    connectthreadpool.cpp \
    parallelserver.cpp \
    userinfo.cpp \
    userinfolist.cpp \
    sendfile.cpp \
    sendfilethread.cpp \
    rdtsender.cpp

HEADERS  += qclient.h \
    signin.h \
    message.h \
    tcpsocketmsg.h \
    signup.h \
    connectthread.h \
    connectthreadpool.h \
    parallelserver.h \
    userinfo.h \
    userinfolist.h \
    sendfile.h \
    sendfilethread.h \
    rdtsender.h

FORMS    += qclient.ui \
    signin.ui \
    signup.ui \
    sendfile.ui

RESOURCES += \
    images.qrc
