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
    ../libovr_nsb/OVR_Sensor.c \
    ../libovr_nsb/OVR_HID.c \
    ../libovr_nsb/OVR_Helpers.c \
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
    ../libovr_nsb/OVR_Sensor.h \
    ../libovr_nsb/OVR_HID.h \
    ../libovr_nsb/OVR_Device.h \
    ../libovr_nsb/OVR_Defs.h \
    ../libovr_nsb/OVR.h \
    urlentrywidget.h \
    envskybox.h

#unix:INCLUDEPATH += ../libovr_nsb ../gl-matrix.c
unix:INCLUDEPATH += ../gl-matrix.c
#unix:LIBS += -L../libovr_nsb -L../gl-matrix.c -lGLU -lovr_nsb -lgl-matrix
unix:LIBS += -L../gl-matrix.c -lGLU -lgl-matrix
