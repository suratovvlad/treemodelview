#-------------------------------------------------
#
# Project created by QtCreator 2016-04-22T15:13:26
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += console

TARGET = treemodelview
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    TreeModel.cpp \
    TreeItem.cpp \
    FileParser.cpp

HEADERS  += MainWindow.h \
    TreeModel.h \
    TreeItem.h \
    FileParser.h

FORMS    += MainWindow.ui
