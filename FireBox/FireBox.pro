#-------------------------------------------------
#
# Project created by QtCreator 2011-05-26T17:27:40
#
#-------------------------------------------------

QT       += core gui opengl network xml multimedia

TARGET = FireBox
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    glwidget.cpp \
    mathutil.cpp \
    game.cpp \
    environment.cpp \
    player.cpp \
    htmlpage.cpp \
    spinanimation.cpp \
    envcell.cpp \
    textgeom.cpp \
    objfile.cpp \
    envroom.cpp \
    envimg.cpp \
    enventrance.cpp \
    envtitle.cpp \
    envtext.cpp \
    envobject.cpp \
    riftrenderer.cpp \
    urlentrywidget.cpp \
    envskybox.cpp \
    environment2.cpp \
    envportalroom.cpp \
    envportal.cpp \
    soundmanager.cpp \
    envroomtemplate.cpp

HEADERS  += mainwindow.h \
    glwidget.h \
    mathutil.h \
    game.h \
    environment.h \
    player.h \
    htmlpage.h \
    spinanimation.h \
    envcell.h \
    textgeom.h \
    objfile.h \
    envroom.h \
    envimg.h \
    enventrance.h \
    envtitle.h \
    envtext.h \
    envobject.h \
    riftrenderer.h \
    urlentrywidget.h \
    envskybox.h \
    environment2.h \
    envportalroom.h \
    envportal.h \
    soundmanager.h \
    envroomtemplate.h

win32:INCLUDEPATH += "../OculusSDK-MinGW-master/LibOVR/Include"
win32:LIBS += -L"..\OculusSDK-MinGW-master\LibOVR\Lib\MinGW\Release\w32"
win32:LIBS += -lovr -lwinmm -lsetupapi

unix:INCLUDEPATH += "../LibOVR/Include"
unix:LIBS += -L"../LibOVR/Lib/Linux/Release/x86_64"
unix:LIBS += -lovr -lX11 -lXinerama -ludev -lGLU
