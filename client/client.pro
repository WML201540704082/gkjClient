QT       += core gui network sql

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
               
LIBS += -L$$OUT_PWD/../lnsoft-otfr/ -ldvtCamera \
        -ladvapi32  \
        -luser32
        
SOURCES += \
    checkpwdwidget.cpp \
    choosefacewidget.cpp \
    detailwidget.cpp \
    faceloginwidget.cpp \
    facemanagewidget.cpp \
    firstloginwidget.cpp \
    floatingerrorwindow.cpp \
    main.cpp \
    mainwindow.cpp \
    model/caminiwork.cpp \
    model/databasemanager.cpp \
    model/http/gettimestampswork.cpp \
    model/http/httpclient.cpp \
    model/http/sendadmininfowork.cpp \
    model/http/sendlogwork.cpp \
    model/http/sendtimeinfowork.cpp \
    model/http/sendtmpuserinfowork.cpp \
    model/mysm4.cpp \
    model/pwdcheck.cpp \
    model/screenstatuscheck.cpp \
    model/sm3/sm3.c \
    model/sm4/gf128.c \
    model/sm4/ghash.c \
    model/sm4/hex.c \
    model/sm4/rand.c \
    model/sm4/sm4.c \
    model/sm4/sm4_cbc.c \
    model/syncwork.cpp \
    mypwdlinedit.cpp \
    picwidget.cpp \
    pwdloginwidget.cpp \
    timewidget.cpp \
    tmpauthmanagewidget.cpp \
    tmpauthorizewidget.cpp \
    viewModel/myviewmodel.cpp

HEADERS += \
    checkpwdwidget.h \
    choosefacewidget.h \
    detailwidget.h \
    faceloginwidget.h \
    facemanagewidget.h \
    firstloginwidget.h \
    floatingerrorwindow.h \
    globalVariables.h \
    mainwindow.h \
    model/caminiwork.h \
    model/databasemanager.h \
    model/http/gettimestampswork.h \
    model/http/httpclient.h \
    model/http/sendadmininfowork.h \
    model/http/sendlogwork.h \
    model/http/sendtimeinfowork.h \
    model/http/sendtmpuserinfowork.h \
    model/myStruct.h \
    model/mysm4.h \
    model/pwdcheck.h \
    model/screenstatuscheck.h \
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
    picwidget.h \
    pwdloginwidget.h \
    timewidget.h \
    tmpauthmanagewidget.h \
    tmpauthorizewidget.h \
    utf8.h \
    viewModel/myviewmodel.h

FORMS += \
    checkpwdwidget.ui \
    choosefacewidget.ui \
    detailwidget.ui \
    faceloginwidget.ui \
    facemanagewidget.ui \
    firstloginwidget.ui \
    floatingerrorwindow.ui \
    mainwindow.ui \
    picwidget.ui \
    pwdloginwidget.ui \
    timewidget.ui \
    tmpauthmanagewidget.ui \
    tmpauthorizewidget.ui
    
RC_ICONS = $$PWD/images/logo3@2x.ico


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
