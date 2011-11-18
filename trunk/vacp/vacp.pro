#-------------------------------------------------
#
# Project created by QtCreator 2011-06-12T11:20:04
#
#-------------------------------------------------

QT       += core

QT       -= gui

QT       += sql

QT       += xml

QT       += xmlpatterns

TARGET = vacp
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    pipeline.cpp \
    build.cpp \
    contentexporter.cpp \
    texturedata.cpp \
    textureexporter.cpp \
    pluginmanager.cpp \
    output_color.cpp \
    package.cpp \
    packagetable.cpp \
    content.cpp

HEADERS += \
    pipeline.h \
    build.h \
    error.h \
    texturedata.h \
    pluginmanager.h \
    plugin_header/vacp_plugin_common.h \
    package.h \
    packageelement.h \
    packagetable.h \
    content.h \
    utilities.h
