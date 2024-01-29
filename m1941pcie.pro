QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    worker.cpp

HEADERS += \
    ../../桌面/test2/libl1941.v.1_0_0/l1941lib.h \
    common.h \
    mainwindow.h \
    worker.h \
    xdma/cdev_sgdma.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    m1941pcietest_zh_CN.ts


unix:!macx: LIBS += -L$$PWD/../m1941lib/lib/ -lm1941

INCLUDEPATH += $$PWD/../m1941lib
DEPENDPATH += $$PWD/../m1941lib

unix:!macx: PRE_TARGETDEPS += $$PWD/../m1941lib/lib/libm1941.a
