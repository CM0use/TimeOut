QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

VERSION = 1.0.0
QMAKE_TARGET_PRODUCT = TimeOut
QMAKE_TARGET_DESCRIPTION = "A timer with customized program/command executions"
QMAKE_TARGET_COPYRIGHT = CM0use

win32: RC_ICONS = icons/appIcon.ico

SOURCES += \
    clock.cpp \
    framelesswindow.cpp \
    main.cpp \
    window.cpp

HEADERS += \
    clock.h \
    framelesswindow.h \
    icons.h \
    window.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    i18n.qrc \
    icons.qrc \
    qss.qrc
