#-------------------------------------------------
#
# Project created by QtCreator 2011-06-12T11:20:04
#
#-------------------------------------------------

QT       += core

QT       -= gui

QT       += sql

TARGET = vacp
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    pipeline.cpp \
    build.cpp \
    asset.cpp \
    contentexporter.cpp \
    texturedata.cpp \
    textureexporter.cpp \
    pluginmanager.cpp

HEADERS += \
    pipeline.h \
    build.h \
    error.h \
    asset.h \
    contentexporter.h \
    texturedata.h \
    textureexporter.h \
    pluginmanager.h
