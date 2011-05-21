#-------------------------------------------------
#
# Project created by QtCreator 2011-05-16T23:37:50
#
#-------------------------------------------------

QT       += core gui

TARGET = maelstrom
TEMPLATE = app
RESOURCES = resources.qrc

win32 {
LIBS += ../../vortex-engine/Tool-debug/vortex-engine.lib
LIBS += -luser32 -lgdi32 -lkernel32 -lwinspool -lcomdlg32 -lole32 -loleaut32 -luuid -lodbc32 -lodbccp32
}
unix {
}

INCLUDEPATH += ../../vortex-engine/include/

SOURCES += main.cpp\
        mainwindow.cpp \
    viewport.cpp \
    splashscreen.cpp

HEADERS  += mainwindow.h \
    viewport.h \
    splashscreen.h

FORMS    += mainwindow.ui \
    viewport.ui \
    splashscreen.ui

RESOURCES += \
    resources.qrc
