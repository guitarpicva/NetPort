#-------------------------------------------------
#
# Project created by QtCreator 2016-12-31T09:00:42
#
#-------------------------------------------------

QT       += core gui serialport network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NetPort
TEMPLATE = app
CONFIG += c++11
RC_ICONS = RCSIcon.ico

SOURCES += main.cpp\
        netport.cpp \
    netportconfigdialog.cpp

HEADERS  += netport.h \
    netportconfigdialog.h

FORMS    += netport.ui \
    netportconfigdialog.ui

RESOURCES += \
    qresources.qrc

