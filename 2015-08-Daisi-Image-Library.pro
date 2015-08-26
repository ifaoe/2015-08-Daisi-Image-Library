#-------------------------------------------------
#
# Project created by QtCreator 2014-01-28T13:34:45
#
#-------------------------------------------------

QT       += core gui sql xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT_QMAKE_EXECUTABLE = /usr/bin/qmake-qt4

CONFIG += console

TARGET = daisi-image-library
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11


VPATH += ./src
SOURCES += main.cpp \
        MainWindow.cpp \
        DatabaseHandler.cpp \
        UserSettings.cpp \
        ImageCanvas.cpp \
        QtExtension/QSqlReadOnlyTableModel.cpp \
        ImagePopup.cpp \
        ColumnChooser.cpp
        
HEADERS += MainWindow.h \
        DatabaseHandler.h \
        UserSettings.h \
        ImageCanvas.h \
        QtExtension/QSqlReadOnlyTableModel.h \
        ImagePopup.h \
        ColumnChooser.h

VPATH += ./ui
FORMS    += mainwindow.ui \
            image_popup.ui\
            column_options.ui

unix: LIBS += -L/usr/lib/ \
            -lqgis_core\
            -lqgis_gui \
            -lgdal
INCLUDEPATH += /usr/include
INCLUDEPATH += /usr/include/qgis
DEPENDPATH += /usr/include
DEPENDPATH += /usr/include/qgis

DEFINES += GUI_EXPORT= CORE_EXPORT=
