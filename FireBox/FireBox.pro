#-------------------------------------------------
#
# Project created by QtCreator 2011-05-26T17:27:40
#
#-------------------------------------------------

QT       += core gui opengl network xml

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
    envskybox.cpp

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
    envskybox.h

unix:INCLUDEPATH += ../LibOVR/Include
unix:LIBS += -L../LibOVR/Lib/Linux/Release/x86_64
unix:LIBS += -lovr -lX11 -lXinerama -ludev -lGLU


