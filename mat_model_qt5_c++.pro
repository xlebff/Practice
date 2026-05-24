#-------------------------------------------------
#
# Project created by QtCreator 2020-09-07T08:29:16
#
#-------------------------------------------------

QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = mat_model_c++
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp

HEADERS  += mainwindow.h \
    SignalParams.h \
    qcustomplot.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    params.ini
LIBS += -lfftw3

DISTFILES += \
    commented.txt
