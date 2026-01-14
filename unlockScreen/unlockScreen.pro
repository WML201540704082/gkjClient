QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_MESSAGELOGCONTEXT

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DESTDIR = $$OUT_PWD/../lnsoft-otfr

INCLUDEPATH += ../dvtCamera \
               $$PWD/model/sm4
               
LIBS += -L$$OUT_PWD/../lnsoft-otfr/ -ldvtCamera

SOURCES += \
    floatingerrorwindow.cpp \
    main.cpp \
    mainwindow.cpp \
    model/caminiwork.cpp \
    model/databasemanager.cpp \
    model/http/getadmininfowork.cpp \
    model/http/gettimeinfowork.cpp \
    model/http/gettimestampswork.cpp \
    model/http/gettmpuserinfowork.cpp \
    model/http/httpclient.cpp \
    model/mysm4.cpp \
    model/sm3/sm3.c \
    model/sm4/gf128.c \
    model/sm4/ghash.c \
    model/sm4/hex.c \
    model/sm4/rand.c \
    model/sm4/sm4.c \
    model/sm4/sm4_cbc.c \
    model/syncwork.cpp \
    mypwdlinedit.cpp \
    viewModel/checkanddecryptwork.cpp \
    viewModel/myviewmodel.cpp

HEADERS += \
    floatingerrorwindow.h \
    globalVariables.h \
    mainwindow.h \
    model/caminiwork.h \
    model/databasemanager.h \
    model/http/getadmininfowork.h \
    model/http/gettimeinfowork.h \
    model/http/gettimestampswork.h \
    model/http/gettmpuserinfowork.h \
    model/http/httpclient.h \
    model/myStruct.h \
    model/mysm4.h \
    model/pwdcheck.h \
    model/sm3/endian.h \
    model/sm3/error.h \
    model/sm3/sm3.h \
    model/sm4/endian.h \
    model/sm4/error.h \
    model/sm4/gf128.h \
    model/sm4/ghash.h \
    model/sm4/hex.h \
    model/sm4/mem.h \
    model/sm4/rand.h \
    model/sm4/sm4.h \
    model/syncwork.h \
    mypwdlinedit.h \
    utf8.h \
    viewModel/checkanddecryptwork.h \
    viewModel/myviewmodel.h

FORMS += \
    floatingerrorwindow.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
