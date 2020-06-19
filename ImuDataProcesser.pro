#-------------------------------------------------
# ImuDataProcesser is used for data segmentation after data acquisition.
# Author : Flame
# Last change: 2022-2-13
# E-mail : sayitagain.flame@qq.com
# Language : C++17
#-------------------------------------------------

QT       += core gui
QT       += multimedia multimediawidgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = ImuDataProcesser
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17
CONFIG += warn_off
#CONFIG += console

SOURCES += \
    ImuMatrix.cpp \
    Main.cpp \
    ImuProcesserWidget.cpp \
    ImageWidget.cpp \
    QCustomplot.cpp

HEADERS += \
    ImuMatrix.h \
    Dataframe.hpp\
    ImuProcesserWidget.h \
    ImageWidget.h\
    FlameList.hpp \
    QCustomplot.h \
    ImuChart.hpp

FORMS += \
    ImuProcesserWidget.ui

RC_ICONS += settings_48px.ico
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
