#-------------------------------------------------
#
# Project created by QtCreator 2011-05-16T23:37:50
#
#-------------------------------------------------

QT       += core gui sql

TARGET = maelstrom
TEMPLATE = app
RESOURCES = resources.qrc
DEFINES += VTX_TOOL_BUILD ASSERTIONS_ENABLED


win32 {
LIBS += ../../Vortex/Tool-debug/vortex-engine.lib
LIBS += -luser32 -lgdi32 -lkernel32 -lwinspool -lcomdlg32 -lole32 -loleaut32 -luuid -lodbc32 -lodbccp32
INCLUDEPATH += ../../Vortex/src
INCLUDEPATH += Forms
INCLUDEPATH += wizards/AssetDB
INCLUDEPATH += wizards/NewProject
INCLUDEPATH += Widgets
}
unix {
LIBS += -L../../../vortex/trunk/src/ -lvortex-tool -lX11 -lGL -lGLU
INCLUDEPATH += ../../../vortex/trunk/include/
INCLUDEPATH += Forms
INCLUDEPATH += wizards/AssetDB
INCLUDEPATH += wizards/NewProject
}

UI_DIR = ../maelstrom/Forms


SOURCES += main.cpp \
    maelstromproject.cpp \
    Wizards/AssetDB/assetdbnewexisting.cpp \
    Wizards/AssetDB/assetdbinfo.cpp \
    Wizards/AssetDB/assetdbwizard.cpp \
    Wizards/AssetDB/assetdbnewsqlite.cpp \
    Forms/resourceeditor.cpp \
    Forms/viewport.cpp \
    Forms/mainwindow.cpp \
    Forms/splashscreen.cpp \
    wizards/newproject/newprojectwizard.cpp \
    wizards/newproject/newprojectfirst.cpp \
    Forms/textinputdialog.cpp \
    Widgets/packagelistwidget.cpp

HEADERS  +=  maelstromproject.h \
    Wizards/AssetDB/assetdbnewexisting.h \
    Wizards/AssetDB/assetdbinfo.h \
    Wizards/AssetDB/assetdbwizard.h \
    Wizards/AssetDB/assetdbnewsqlite.h \
    Forms/resourceeditor.h \
    Forms/viewport.h \
    Forms/mainwindow.h \
    Forms/splashscreen.h \
    wizards/newproject/newprojectwizard.h \
    wizards/newproject/newprojectfirst.h \
    Forms/textinputdialog.h \
    Widgets/packagelistwidget.h

FORMS    += mainwindow.ui \
    viewport.ui \
    splashscreen.ui \
    resourceeditor.ui \
    Forms/textinputdialog.ui \
    Widgets/packagelistwidget.ui

RESOURCES += \
    resources.qrc
