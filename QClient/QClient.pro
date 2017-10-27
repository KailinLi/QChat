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
    handledir.cpp \
    sendfile.cpp \
    receivefile.cpp \
    sendfilethread.cpp \
    receivefilethread.cpp \
    rdtsender.cpp \
    rdtreceiver.cpp

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
    receivefile.h \
    sendfilethread.h \
    receivefilethread.h \
    rdtsender.h \
    rdtreceiver.h

FORMS    += qclient.ui \
    signin.ui \
    signup.ui \
    sendfile.ui \
    receivefile.ui

RESOURCES += \
    images.qrc
